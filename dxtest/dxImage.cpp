#include "dxImage.h"
#include <Windows.h>
#include "sleep.h"

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment( lib, "gdiplus.lib" )

LPDIRECT3DTEXTURE9 loadtex(LPCTSTR pSrcFile,bool mask,unsigned int &width,unsigned int &height);

LPDIRECT3DVERTEXBUFFER9 g_pVB;                  //���㻺��
LPDIRECT3DVERTEXBUFFER9 g_pVB2;                 //ת�䶥�㻺��
IDirect3DPixelShader9* MultiTexPS = 0;
ID3DXConstantTable* MultiTexCT    = 0;


//��ɫ�����
D3DXHANDLE sample0Handle = 0;
D3DXHANDLE sample1Handle = 0;
D3DXHANDLE sample2Handle = 0;
D3DXHANDLE alphaHandle = 0;
D3DXHANDLE alpha_oldHandle = 0;
D3DXHANDLE alpha_newHandle = 0;

D3DXCONSTANT_DESC sample0Desc;
D3DXCONSTANT_DESC sample1Desc;
D3DXCONSTANT_DESC sample2Desc;

texlist *headp;
texlist *nowp;

bool InitVertex()
{
	//�������㻺��
	if( FAILED(g_pd3dD->CreateVertexBuffer(sizeof(Vertex)*4, 0, VertexFVF, D3DPOOL_MANAGED, &g_pVB, NULL)) )
		return false;

	Vertex* pVertices;
	if( FAILED(g_pVB->Lock( 0, 0, (void**)&pVertices, 0 )) )
		return false;

	pVertices[0] = Vertex(0.0f, 0.0f       ,       0.0f, 0.0f);
	pVertices[1] = Vertex(0.0f, 1.0f       ,       0.0f, 1.0f);
	pVertices[2] = Vertex(1.0f, 0.0f       ,       1.0f, 0.0f);
	pVertices[3] = Vertex(1.0f, 1.0f       ,       1.0f, 1.0f);
	g_pVB->Unlock();

	//�������㻺��2
	if( FAILED(g_pd3dD->CreateVertexBuffer(sizeof(Vertex2)*4, 0, Vertex2FVF, D3DPOOL_MANAGED, &g_pVB2, NULL)) )
		return false;

	Vertex2* pVertices2;
	if( FAILED(g_pVB2->Lock( 0, 0, (void**)&pVertices2, 0 )) )
		return false;

	pVertices2[0] = Vertex2(0.0f, 0.0f       ,       0.0f, 0.0f,0.0f, 0.0f);
	pVertices2[1] = Vertex2(0.0f, 1.0f       ,       0.0f, 1.0f,0.0f, 1.0f);
	pVertices2[2] = Vertex2(1.0f, 0.0f       ,       1.0f, 0.0f,1.0f, 0.0f);
	pVertices2[3] = Vertex2(1.0f, 1.0f       ,       1.0f, 1.0f,1.0f, 1.0f);
	g_pVB2->Unlock();

	return true;
}

void ReleaseVertex()
{
	//�ͷŶ��㻺��
	if( g_pVB != NULL )
		g_pVB->Release();
	if( g_pVB2 != NULL )
		g_pVB2->Release();
}

//��ʼ��������ɫ��
bool InitEffect()
{
	HRESULT rlt;
	ID3DXBuffer* shader      = NULL;
	ID3DXBuffer* errorBuffer = NULL;
//*	
	rlt = D3DXCompileShaderFromFile(
		L"effect1.txt",0,0,
		"effect1","ps_2_0",0,&shader,&errorBuffer,&MultiTexCT);
//*/

/*	rlt = D3DXCompileShader(
		Effects,
		806,
		0,
		0,
		"effect1",
		"ps_2_0",
		0, 
		&shader,
		&errorBuffer,
		&MultiTexCT);
//*/

	if( errorBuffer )
	{
		::MessageBoxA(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		errorBuffer->Release();
		return false;
	}
	if(FAILED(rlt))
	{
		::MessageBoxA(0,"D3DXCompileShader failed!", 0, 0);
		return false;
	}


	if(FAILED(g_pd3dD->CreatePixelShader((DWORD*)shader->GetBufferPointer(),&MultiTexPS)))
	{
		::MessageBoxA(0,"CreatePixelShader failed!", 0, 0);
		return false;
	}
	shader->Release();


	MultiTexCT->SetDefaults(g_pd3dD);

	alphaHandle = MultiTexCT->GetConstantByName(0, "alpha");
	sample0Handle = MultiTexCT->GetConstantByName(0, "Samp0");
	sample1Handle = MultiTexCT->GetConstantByName(0, "Samp1");

	UINT count;
	MultiTexCT->GetConstantDesc(sample0Handle, &sample0Desc, &count);
	MultiTexCT->GetConstantDesc(sample1Handle, &sample1Desc, &count);

	return true;
}

void ReleaseEffect()
{
	if( MultiTexPS != NULL )
		MultiTexPS->Release();
	if( MultiTexCT != NULL )
		MultiTexCT->Release();
}

//=============================================================
//                         dximg��
//=============================================================
dximg::dximg()
{
	g_pTexture = NULL;
	nextTex = NULL;

	nowx = 0.0f;
	nowy = 0.0f;
}

dximg::~dximg()
{
	if( g_pTexture != NULL )
		g_pTexture->Release();
	if( nextTex != NULL )
		nextTex->Release();
}

bool dximg::loadimg(LPCTSTR pSrcFile,bool mask)              //��ȡͼƬ
{
	D3DXMatrixIdentity( &scale );
	D3DXMatrixIdentity( &trans );
	D3DXMatrixIdentity( &rot );

	if(g_pTexture=loadtex(pSrcFile,mask,width,height))
	{
		setWH(width,height);
		return true;
	}
	else
		return false;
}

bool dximg::loadnextimg(LPCTSTR nextFile,bool mask)              //��ȡͼƬ
{
	D3DXMatrixIdentity( &scale2 );
	D3DXMatrixIdentity( &trans2 );

	if(nextTex=loadtex(nextFile,mask,width2,height2))
	{
		setWH2(width2,height2);
		return true;
	}
	else
		return false;
}

void dximg::setuv()
{
	//������ͼ�ĳ���
	float w,h;
	float x0,y0;

	//��������ͼ��������
	float x1[4],x2[4],y1[4],y2[4];

	x1[0]=nowx;y1[0]=nowy;
	x1[1]=nowx;y1[1]=nowy+height;
	x1[2]=nowx+width;y1[2]=nowy;
//	x1[3]=nowx+height;y1[3]=nowy+width;

	x2[0]=nowx2;y2[0]=nowy2;
	x2[1]=nowx2;y2[1]=nowy2+height2;
	x2[2]=nowx2+width2;y2[2]=nowy2;
//	x2[3]=nowx2+height2;y2[3]=nowy2+width2;

	x0= (x1[0]<x2[0]?x1[0]:x2[0]);
	y0= (y1[0]<y2[0]?y1[0]:y2[0]);
	w = (x1[2]>x2[2]?x1[2]:x2[2]) - x0;
	h = (y1[1]>y2[1]?y1[1]:y2[1]) - y0;

	x1[0] = (x0  -nowx)/width;y1[0] = (y0  -nowy)/height;
	x1[1] = (x0  -nowx)/width;y1[1] = (y0+h-nowy)/height;
	x1[2] = (x0+w-nowx)/width;y1[2] = (y0  -nowy)/height;
	x1[3] = (x0+w-nowx)/width;y1[3] = (y0+h-nowy)/height;
	
	x2[0] = (x0  -nowx2)/width2;y2[0] = (y0  -nowy2)/height2;
	x2[1] = (x0  -nowx2)/width2;y2[1] = (y0+h-nowy2)/height2;
	x2[2] = (x0+w-nowx2)/width2;y2[2] = (y0  -nowy2)/height2;
	x2[3] = (x0+w-nowx2)/width2;y2[3] = (y0+h-nowy2)/height2;

	//�޸Ķ��㻺��,��д�ڶ�����������
	Vertex2* pVtemp;
	g_pVB2->Lock( 0, 0, (void**)&pVtemp, 0 );
	pVtemp[0] = Vertex2(0.0f, 0.0f       ,       x1[0], y1[0],x2[0], y2[0]);
	pVtemp[1] = Vertex2(0.0f, 1.0f       ,       x1[1], y1[1],x2[1], y2[1]);
	pVtemp[2] = Vertex2(1.0f, 0.0f       ,       x1[2], y1[2],x2[2], y2[2]);
	pVtemp[3] = Vertex2(1.0f, 1.0f       ,       x1[3], y1[3],x2[3], y2[3]);
	g_pVB2->Unlock();

	setWH(w,h);
	setpos(x0,y0);
}

void dximg::setpos(float x,float y)                    //����ͼƬ����
{
	this;
	nowx = (int)x; nowy = (int)y;
	D3DXMatrixTranslation(&trans, nowx-0.5f, nowy-0.5f, 0);
}

void dximg::move(float x,float y)                      //�ƶ�ͼƬ
{
	D3DXMatrixTranslation(&trans, (int)(nowx+x)-0.5f, (int)(nowy+y)-0.5f, 0); 
	nowx=nowx+x;
	nowy=nowy+y;
}

void dximg::setzoom(float zoomx,float zoomy)           //����ͼƬ
{
	width=width*zoomx;
	height=height*zoomy;
	D3DXMatrixScaling(&scale,width,height,0);
}

void dximg::setWH(float _width,float _height)         //����ͼƬ����
{
	if(_width&&_height)
	{
		width = _width;
		height = _height;
	}
	D3DXMatrixScaling(&scale,width,height,0);
}

void dximg::setrota(float rotation)                    //˳ʱ����תͼƬ
{
	D3DXMatrixRotationZ(&rot, rotation *D3DX_PI/180);
}

void dximg::setpos2(float x,float y)                    //����nextͼƬ����
{
	nowx2 = (int)x; nowy2 = (int)y;
	D3DXMatrixTranslation(&trans2, nowx2-0.5f, nowy2-0.5f, 0);
	setuv();
}

void dximg::setWH2(float _width,float _height)      //����nextͼƬ����
{
	if(_width&&_height)
	{
		width2 = _width;
		height2 = _height;
	}
	D3DXMatrixScaling(&scale2,width2,height2,0);
}

void dximg::draw(double alpha)                                     //����ͼƬ
{
	g_pd3dD->SetTransform(D3DTS_WORLD,&(scale*trans));                      //�����������(rot)

	g_pd3dD->SetTexture( 0, g_pTexture );

	if(nextTex==NULL)
	{
		if(alpha!=255)
		{
			g_pd3dD->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0,0,0,int(255.0-alpha)) );
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SUBTRACT);      //���
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		}
		else
		{
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SELECTARG1);
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		}

			g_pd3dD->SetStreamSource( 0, g_pVB, 0, sizeof( Vertex ) );
			g_pd3dD->SetFVF( VertexFVF );
			g_pd3dD->DrawPrimitive( D3DPT_TRIANGLESTRIP ,0, 2 );
	}

	else
	{
		if(alpha>0.0)
		{
			MultiTexCT->SetFloat( g_pd3dD, alphaHandle, 1.0-(alpha/255.0) );               //����alphaֵ
			g_pd3dD->SetPixelShader( MultiTexPS );

			g_pd3dD->SetTexture( sample0Desc.RegisterIndex, g_pTexture);
			g_pd3dD->SetTexture( sample1Desc.RegisterIndex, nextTex );
			g_pd3dD->SetTextureStageState(sample0Desc.RegisterIndex,D3DTSS_TEXCOORDINDEX,0);
			g_pd3dD->SetTextureStageState(sample1Desc.RegisterIndex,D3DTSS_TEXCOORDINDEX,1);


			g_pd3dD->SetStreamSource( 0, g_pVB2, 0, sizeof(Vertex2) );
			g_pd3dD->SetFVF( Vertex2FVF );
			g_pd3dD->DrawPrimitive( D3DPT_TRIANGLESTRIP ,0, 2 );

			g_pd3dD->SetPixelShader( NULL );

		}
		else if(alpha==0.0)        //ת�����=======================================================
		{
			if( g_pTexture != NULL )
				g_pTexture->Release();	//�ͷ�ԭ��ͼ
			g_pTexture=nextTex;			//��ԭ��ͼָ��ָ��next��ͼ
			nextTex=NULL;				//nextTexָ��NULL
			scale=scale2;trans=trans2;width=width2;height=height2;nowx=nowx2;nowy=nowy2;
			//================================================

			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SELECTARG1);		//���
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

			g_pd3dD->SetTransform(D3DTS_WORLD,&(scale*trans));							//�����������
			g_pd3dD->SetTexture( 0, g_pTexture );

			g_pd3dD->SetStreamSource( 0, g_pVB, 0, sizeof( Vertex ) );
			g_pd3dD->SetFVF( VertexFVF );
			g_pd3dD->DrawPrimitive( D3DPT_TRIANGLESTRIP ,0, 2 );
		}

	}

	g_pd3dD->SetTexture(0, NULL);
}

void changeImg(dximg *a,dximg *b)
{
	//��������
	if( a->g_pTexture != NULL )
		a->g_pTexture->Release();
	a->g_pTexture = b->g_pTexture;
	a->width = b->width;
	a->height = b->height;
	a->nowx = b->nowx;
	a->nowy = b->nowy;
	a->setWH(a->width,a->height);
	a->setpos(a->nowx,a->nowy);

	if( b->g_pTexture != NULL )
		b->g_pTexture = NULL;
}

bool fadein(float &alpha,float step)
{
	if(alpha!=255.0f)
	{
		alpha+=step;
		if(alpha>255.0f)
			alpha=255.0f;
		return false;
	}
	else
		return true;
}

bool fadeout(float &alpha,float step)
{
	if(alpha!=0.0f)
	{
		alpha-=step;
		if(alpha<0.0f)
			alpha=0.0f;
		return false;
	}
	else
		return true;
}

bool loadmaskimg(LPCTSTR File,LPDIRECT3DTEXTURE9 &tex,unsigned int &width,unsigned int &height)
{
	int i=0;

	//��ȡ�ļ�
	Bitmap scrBit(File);
	if(scrBit.GetLastStatus()!=Ok)
	{
		MessageBox(0,File,TEXT("��ȡͼƬ����"),MB_OK);
		return false;
	}

	//��ÿ��
	width = scrBit.GetWidth()/2;
	height = scrBit.GetHeight();

	long bytenum = width*height*4;
	byte* masked_pixel_buf = new byte[bytenum];

	Rect rect( 0, 0, width*2, height );
	BitmapData *destBit = new BitmapData;
	scrBit.LockBits( &rect, ImageLockModeRead, PixelFormat24bppRGB, destBit );

	BYTE *pByte = (BYTE *) destBit->Scan0;
	BYTE *pmaskByte = (BYTE *) destBit->Scan0 + width*3;

	int  iOffset = destBit->Stride - width*3;    //ÿ�е��ֽ���

	for( int iRow=0; iRow<height; iRow++ )
	{
		for( int iCol=0; iCol<width; iCol++ )
		{
			masked_pixel_buf[i] = pByte[0];
			masked_pixel_buf[i+1] = pByte[1];
			masked_pixel_buf[i+2] = pByte[2];
			masked_pixel_buf[i+3] = 0xff - pmaskByte[0];

			i += 4;
			pByte += 3;
			pmaskByte += 3;
		}
		pByte += iOffset;
		pmaskByte += iOffset;
	}

	scrBit.UnlockBits( destBit );



	if (g_pd3dD->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex, NULL) == D3D_OK)
	{
		//��tex���д���
		D3DLOCKED_RECT locked_rect;
		tex->LockRect(0, &locked_rect, NULL, 0);

		//ֱ�Ӹ����ڴ��
		CopyMemory(locked_rect.pBits,masked_pixel_buf,bytenum);

		tex->UnlockRect(0);
	}

	return true;
}

LPDIRECT3DTEXTURE9 loadtex(LPCTSTR pSrcFile,bool mask,unsigned int &width,unsigned int &height)
{
	texlist *sch=headp;
	while(sch)
	{
		if(!wcscmp(sch->pSrcFile,pSrcFile))
			return sch->g_pTexture;
		sch = sch->next;
	}

	texlist *temp = new texlist;

	//��ȡͼƬ��Ϣ
	D3DXIMAGE_INFO imageInfo;
	D3DXGetImageInfoFromFile(pSrcFile, &imageInfo);
	width = imageInfo.Width;
	height = imageInfo.Height;

	if(!mask)
	{
		//���ļ��д�����ͼ
		if(FAILED(D3DXCreateTextureFromFileEx(
			g_pd3dD,
			pSrcFile,
			imageInfo.Width,
			imageInfo.Height,
			imageInfo.MipLevels,
			0,
			imageInfo.Format,
			D3DPOOL_DEFAULT,
			D3DX_FILTER_NONE,
			D3DX_DEFAULT,
			0,
			NULL,
			NULL,
			&temp->g_pTexture)))
		{
			MessageBox(0,pSrcFile,TEXT("��ȡͼƬ����"),MB_OK);
			return 0;
		}
	}
	else
	{
		if(!loadmaskimg(pSrcFile,temp->g_pTexture,width,height))
			return 0;
	}

	if(!headp)
	{
		headp = temp;
		nowp = headp;
		nowp->next = 0;
	}
	else
	{
		nowp->next = temp;
		nowp = temp;
		nowp->next = 0;
	}

	return nowp->g_pTexture;
}