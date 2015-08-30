#pragma once

#include <d3dx9.h>
#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"
//#pragma comment(lib , "freetype248_D.lib")

bool initFontVB();
bool initFontFile();
void ReleaseFontVB();

// �洢һ���ַ�ͼƬ��صĽṹ��
struct SCharTexture
{
	LPDIRECT3DTEXTURE9 m_Texture;	// ����
	wchar_t m_chaID;				// ����
	int     m_Width;				// ��
	int     m_Height;				// ��

	int     m_adv_x;				
	int     m_adv_y;				
	int     m_delta_x;				
	int     m_delta_y;				

	SCharTexture()
	{
		m_Texture= 0;
		m_chaID  = 0;
		m_Width  = 0;
		m_Height = 0;
	}
};

class CFreeTypeLib
{
public:
	CFreeTypeLib(IDirect3DDevice9* pDevice);
	~CFreeTypeLib(void);
	
	// ��������
	bool load(const char* font_file , int _w , int _h);

	//�ı������С
	bool ChangeSize(int _w , int _h);

	// ��FreeType��Ⱦ�ַ���
	void DrawText(wchar_t* _strText,int x , int y, int maxW , int h=0, bool center=false,D3DCOLOR color=0xffffffff);

private:
	// ��ȡ�ַ�
	SCharTexture* getTextChar(wchar_t ch);

	IDirect3DDevice9* m_pDevice;
	SCharTexture m_TexID[256*256];

	FT_Library m_FT2Lib;
	FT_Face    m_FT_Face;

	int   m_w;	// �ֿ�
	int   m_h;	// �ֳ�
};
