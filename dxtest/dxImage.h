#pragma once
#include <d3dx9.h>

extern LPDIRECT3DDEVICE9 g_pd3dD;               //D3D设备，用于渲染

//用于单张图片显示
struct Vertex
{
	Vertex(){}
	Vertex(float _x, float _y,
		float _tu, float _tv)
	{
		x  = _x;  y  = _y;  z  = 0.0f;
		tu = _tu;  tv  = _tv;
	}
    FLOAT x, y, z; // The position
    FLOAT tu, tv;   // The texture coordinates
};
#define VertexFVF (D3DFVF_XYZ|D3DFVF_TEX1)

//用于两张纹理之间的切换
struct Vertex2
{
	Vertex2(){}
	Vertex2(float _x, float _y,
		float _tu, float _tv, float _tu2, float _tv2)
	{
		x  = _x;  y  = _y;  z  = 0.0f;
		tu = _tu;  tv  = _tv;
		tu2 = _tu2;  tv2  = _tv2;
	}
    FLOAT x, y, z; // The position
    FLOAT tu, tv, tu2, tv2;   // The texture coordinates
};
#define Vertex2FVF (D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_TEX2)

class dximg
{
public:
	dximg();
	~dximg();

	bool loadimg(LPCTSTR pSrcFile,bool mask=false);
	bool loadnextimg(LPCTSTR nextFile,bool mask=false);
	void setpos(float x,float y);
	void setpos2(float x,float y);
	void move(float x,float y);
	void setzoom(float zoomx,float zoomy);
	void setWH(float width,float height);
	void setWH2(float width,float height);
	void setrota(float rotation);

	void draw(double alpha=255);
public:
	unsigned int width,height;
	unsigned int width2,height2;
	float nowx,nowy;
	float nowx2,nowy2;
	LPDIRECT3DTEXTURE9 g_pTexture;             //纹理指针 
	LPDIRECT3DTEXTURE9 nextTex;
private:
	void setuv();
	bool loadmaskimg(LPCTSTR File,LPDIRECT3DTEXTURE9 &tex,unsigned int &width,unsigned int &height);
	D3DXMATRIXA16      trans, rot, scale;
	D3DXMATRIXA16      trans2, scale2;
};

//=======================================
//           纹理储存列表
//=======================================
struct texlist
{
	wchar_t pSrcFile[MAX_PATH];
	LPDIRECT3DTEXTURE9 g_pTexture;
	texlist *next;
};

bool InitVertex();
bool InitEffect();
void ReleaseVertex();
void ReleaseEffect();
bool fadein(float&,float=7.0f);   //10.0f
bool fadeout(float&,float=7.0f);
void changeImg(dximg *a,dximg *b);

static char Effects[]=
"float alpha;\n\
sampler Samp0;\n\
sampler Samp1;\n\
struct PS_INPUT\n\
{\n\
    float2 uvCoords0 : TEXCOORD0;\n\
    float2 uvCoords1 : TEXCOORD1;\n\
};\n\
float4 effect1(PS_INPUT input) : COLOR\n\
{\n\
	float4 texcolor = tex2D(Samp0, input.uvCoords0);\n\
	float4 texcolor2 = tex2D(Samp1, input.uvCoords1);\n\
	texcolor.r = texcolor2.r * alpha + texcolor.r * (1.0 - alpha);\n\
	texcolor.g = texcolor2.g * alpha + texcolor.g * (1.0 - alpha);\n\
	texcolor.b = texcolor2.b * alpha + texcolor.b * (1.0 - alpha);\n\
	texcolor.a = texcolor2.a * alpha + texcolor.a * (1.0 - alpha);\n\
	return texcolor;\n\
}\n";