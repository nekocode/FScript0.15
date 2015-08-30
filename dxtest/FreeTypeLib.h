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

// 存储一个字符图片相关的结构体
struct SCharTexture
{
	LPDIRECT3DTEXTURE9 m_Texture;	// 纹理
	wchar_t m_chaID;				// 编码
	int     m_Width;				// 宽
	int     m_Height;				// 高

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
	
	// 加载字体
	bool load(const char* font_file , int _w , int _h);

	//改变字体大小
	bool ChangeSize(int _w , int _h);

	// 用FreeType渲染字符串
	void DrawText(wchar_t* _strText,int x , int y, int maxW , int h=0, bool center=false,D3DCOLOR color=0xffffffff);

private:
	// 获取字符
	SCharTexture* getTextChar(wchar_t ch);

	IDirect3DDevice9* m_pDevice;
	SCharTexture m_TexID[256*256];

	FT_Library m_FT2Lib;
	FT_Face    m_FT_Face;

	int   m_w;	// 字宽
	int   m_h;	// 字长
};
