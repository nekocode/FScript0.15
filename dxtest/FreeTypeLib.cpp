#include "FreeTypeLib.h"

// �������ݸ�ʽ
struct FontVertex
{
	float _x, _y, _z;
	float u,v;
	DWORD color;

	FontVertex(float x, float y, float tx, float ty)
	{
		_x = x; _y = y;	_z = 0.0f;
		u = tx; v = ty;
	}
};
DWORD vertex_fvf = D3DFVF_XYZ |D3DFVF_TEX1;


IDirect3DVertexBuffer9* Triangle = 0;   //���㻺��
D3DXMATRIXA16 pos;          //�������(��������)
D3DXMATRIXA16 wh;          //�������(�����ַ���С)
extern LPDIRECT3DDEVICE9 g_pd3dD;

//FT_Library m_FT2Lib;

//========================
//initFontVB
//��ʼ�����干�ö��㻺��
//========================
bool initFontVB()
{
	FontVertex *v;     //���干�ö���

	if( FAILED(g_pd3dD->CreateVertexBuffer(4*sizeof(FontVertex),0,vertex_fvf,D3DPOOL_MANAGED,&Triangle,NULL)))
		return false;

	if( FAILED(Triangle->Lock(0,0,(void **)&v,0)))
		return false;

	v[0] = FontVertex(0.0f, 0.0f, 0.0f, 0.0f );
	v[1] = FontVertex(0.0f, 1.0f, 0.0f, 1.0f );
	v[2] = FontVertex(1.0f, 0.0f, 1.0f, 0.0f );
	v[3] = FontVertex(1.0f, 1.0f, 1.0f, 1.0f );
	Triangle->Unlock();

	return true;
}

void ReleaseFontVB()
{
	//�ͷŶ��㻺��
	if( Triangle != NULL )
        Triangle->Release();
}

/*
bool initFontFile(const char* font_file)
{
	FT_Error error = FT_New_Face( m_FT2Lib, font_file, 0, &m_FT_Face );
	if (error)
	{
		::MessageBox(0,L"�����ֿ��ļ�ʧ��",0,0);
		exit(0);
	}
}*/

//===============================================================================
CFreeTypeLib::CFreeTypeLib(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
	
	// ��ʼ��FT��
	FT_Error error = FT_Init_FreeType( &m_FT2Lib);
	if (error)
	{
		::MessageBox(0,L"�����ֿ��ļ�ʧ��",0,0);
		exit(0);
	}
}

CFreeTypeLib::~CFreeTypeLib(void)
{
	for (int i = 0; i < 65535; i++)
	{
		if(m_TexID[i].m_Texture)
			m_TexID[i].m_Texture->Release();
	}
	FT_Done_FreeType(m_FT2Lib);
}


bool CFreeTypeLib::load(const char* font_file , int _w , int _h)
{
	//����һ������,ȡĬ�ϵ�Face,һ��ΪRegualer
	FT_Error error = FT_New_Face( m_FT2Lib, font_file, 0, &m_FT_Face );
	if (error)
	{
		::MessageBox(0,L"�����ֿ��ļ�ʧ��",0,0);
		exit(0);
	}

	m_w = _w ; m_h = _h;
	error = FT_Select_Charmap(m_FT_Face, FT_ENCODING_UNICODE);
	if (error)
	{
		::MessageBox(0,L"ѡ��Unicode����ʧ��",0,0);
		exit(0);
	}

	error = FT_Set_Pixel_Sizes(m_FT_Face,m_w, m_h);
	if (error)
	{
		::MessageBox(0,L"���ô�Сʧ��",0,0);
		exit(0);
	}
	return true;
}

bool CFreeTypeLib::ChangeSize(int _w , int _h)
{
	FT_Error error = FT_Set_Pixel_Sizes(m_FT_Face,m_w, m_h);
	if (error)
	{
		::MessageBox(0,L"���ô�Сʧ��",0,0);
		exit(0);
	}
	//�ͷ������Ѵ��ڵ����建��
	for (int i = 0; i < 65535; i++)
	{
		if(m_TexID[i].m_Texture)
			m_TexID[i].m_Texture->Release();
	}
	return true;
}

//================================================================================================

SCharTexture* CFreeTypeLib::getTextChar(wchar_t ch)
{
	if(m_TexID[ch].m_Texture)         //����Ѿ�������ֱ�ӷ���
		return &m_TexID[ch];

	// װ��TTF����glyph
	FT_Error error = FT_Load_Char(m_FT_Face, ch,FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT );
	if(error)
	{
		return NULL;
	}

	SCharTexture& charTex = m_TexID[ch];
	charTex.m_chaID = ch;

	FT_GlyphSlot slot = m_FT_Face->glyph;
	FT_Bitmap bitmap = slot->bitmap;


	//��λͼ���ݿ����Լ��������������.�����ɿ��Ի�����Ҫ�Ķ��������ˡ�
	int width  =  bitmap.width;
	int height =  bitmap.rows;

	charTex.m_Width = width;
	charTex.m_Height = height;
	charTex.m_adv_x = (int)(m_FT_Face->glyph->advance.x / 64.0f);
	charTex.m_adv_y = m_FT_Face->size->metrics.y_ppem;
	charTex.m_delta_x = (slot->metrics.horiBearingX >> 6);
	charTex.m_delta_y = slot->bitmap_top - height;

	LPDIRECT3DTEXTURE9 d3d9_texture = NULL;
	if (m_pDevice->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &d3d9_texture, NULL) == D3D_OK)
	{
		D3DLOCKED_RECT locked_rect;
		d3d9_texture->LockRect(0, &locked_rect, NULL, 0);
		switch (m_FT_Face->glyph->bitmap.pixel_mode)
		{
		case FT_PIXEL_MODE_GRAY:
			{
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						unsigned char _vl =  (x>=bitmap.width || y>=bitmap.rows) ? 0 : bitmap.buffer[x + bitmap.width*y];
						byte* destination_pixel = ((byte*) locked_rect.pBits) + locked_rect.Pitch * y + x * 4;

						destination_pixel[0] = 0x00; // b
						destination_pixel[1] = 0x00; // g
						destination_pixel[2] = 0x00; // r
						destination_pixel[3] = _vl; // a 
					}
				}
			}
			break;
		case FT_PIXEL_MODE_MONO:
			{	
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						unsigned char _vl = 0;
						if(bitmap.buffer[y*bitmap.pitch + x/8] & (0x80 >> (x & 7)))
							_vl = 0xFF;
						else
							_vl = 0x00;

						byte* destination_pixel = ((byte*) locked_rect.pBits) + locked_rect.Pitch * y + x * 4;
						destination_pixel[0] = 0x00;
						destination_pixel[1] = 0x00;
						destination_pixel[2] = 0x00;
						destination_pixel[3] = _vl;

					}
				}
			}
			break;
		}

		d3d9_texture->UnlockRect(0);
	}
	charTex.m_Texture = d3d9_texture;
	return &charTex;
}

void CFreeTypeLib::DrawText(wchar_t* _strText,int x , int y, int maxW , int offsetH, bool center,D3DCOLOR color)
{
	int sx = x;             //��ǰ����x����
	int sy = y;             //��ǰ����y����
	int char_w,char_h;                //�ַ����
	float ch_x,ch_y;        //ÿ���ַ����xy����
	int offsetX = 0;        //����xƫ����
	int maxH = m_h + offsetH;       //�߶�ƫ����
	int len = wcslen(_strText);
	SCharTexture* pCharTex;

	if(center)
	{
		for(unsigned int i = 0 ; i < len ; i ++)
		{
			pCharTex = getTextChar(_strText[i]);
			offsetX += pCharTex->m_Width;
		}
		offsetX = -(offsetX/2);
		sx = sx + offsetX;
	}

	for(unsigned int i = 0 ; i < len ; i ++)
	{
		if(_strText[i] =='\n')
		{
			sx = x ; sy += maxH ;
			continue;
		}
		pCharTex = getTextChar(_strText[i]);

		char_w = pCharTex->m_Width;
		char_h = pCharTex->m_Height;

		ch_x = sx + pCharTex->m_delta_x - 0.5f;
		ch_y = sy - char_h - pCharTex->m_delta_y  -0.5f + m_h;

		D3DXMatrixScaling(&wh,char_w,char_h,0.0f);
		D3DXMatrixTranslation(&pos, ch_x, ch_y, 0.0f);
		m_pDevice->SetTransform(D3DTS_WORLD,&(wh*pos));

		m_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, color);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SUBTRACT);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP , D3DTOP_ADD);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

		m_pDevice->SetStreamSource(0,Triangle,0,sizeof(FontVertex));
		m_pDevice->SetFVF(vertex_fvf);
		m_pDevice->SetTexture(0,pCharTex->m_Texture);

		// ��Ⱦ����
		m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

		sx += pCharTex->m_adv_x;
		if(sx > x + maxW)
		{
			sx = x ; sy += maxH;
		}
	}

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP , D3DTOP_SELECTARG1);
}