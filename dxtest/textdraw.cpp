#include "textdraw.h"
#include "sleep.h"            //�Լ�д��time��,���ڿ����ٶ�
#include <tchar.h>

bool echoNew = false;
bool echoAllShow = false;
extern IDirect3DDevice9* g_pd3dD;
dxtext fpsxText,echoxText,rnxText;
dxtext text;

int echospeed=50;             //echoText���ٶ�
bool echoruned = false;
time time1;

//=============����==================
int echosize=25,echoposx,echoposy,echowidth,echooffset=0,echocolor=0xffffffff,rolenamesize=25,rolenameposx,rolenameposy,rolenamewidth,rolenameoffset=0,rolenamecolor=0xffffff;
bool echocenter=false,rolenamecenter=true;
//===================================

//CFreeTypeLib* g_pFreeTypeLib= NULL;

bool dxtext::init(int height,int width)
{
//	if(!g_pFreeTypeLib)
//	{
//	g_pFreeTypeLib->~CFreeTypeLib();
	delete g_pFreeTypeLib;
	g_pFreeTypeLib = new CFreeTypeLib(g_pd3dD);
	if(!g_pFreeTypeLib->load(FontLibName,width,height))
		return false;
//	}
//	else
//	{
//		if(!g_pFreeTypeLib->ChangeSize(height,width))
//			return false;
//	}

	return true;
}

void dxtext::draw(wchar_t *string,int x,int y,int width,int offsetH,bool center,D3DCOLOR color)
{
	//D3DCOLOR_RGBA(0,0,0,255)
	g_pFreeTypeLib->DrawText(string,x,y,width,offsetH,center,color);
}


bool initText(bool fisrt)
{
	//��ʼ�����干�ö��㻺��
	if(fisrt)
	{
		if(!initFontVB())
			return false;

		text.init(20,20);
		fpsxText.init(20,20);
	}

	echoxText.init(echosize,echosize);
	rnxText.init(rolenamesize,rolenamesize);
	return true;
}

void ReleaseText()
{
//	if (g_pFreeTypeLib) g_pFreeTypeLib->~CFreeTypeLib();
	ReleaseFontVB();
}

//==================================
//  ����:echo(wchar_t*,int,int)
//  Ŀ��:���FPSֵ
//
void echo(wchar_t *str)
{
	static long i = 0;
	static wchar_t outText[DefaultBufLen];

	if(echoNew)
	{
		i = 0;
		echoruned = true;
		time1.count = 0.0;

		if(*(str)!='\0')
		{
			_tcscpy_s(outText,DefaultBufLen,str);
			outText[1] = '\0';
			i++;
		}
		else    //�Ѿ�����β
		{
			i = 0;
			echoruned = false;
		}
		echoNew = false;
	}
	if(echoAllShow)
	{
		_tcscpy_s(outText,DefaultBufLen,str);

		i =0 ;
		echoruned = false;
		time1.count = 0.0;
		echoAllShow=false;
	}

	if(echoruned&&time1.sleep(echospeed))
	{
		if(*(str+i)!='\0')
		{
			_tcscpy_s(outText,DefaultBufLen,str);
			outText[i+1] = '\0';
			i++;
		}
		else    //�Ѿ�����β
		{
			i = 0;
			echoruned = false;
		}
	}
	echoxText.draw(outText,echoposx,echoposy,echowidth,echooffset,echocenter,echocolor);
}

//==================================
//  ����:drawrolename(wchar_t*)
//  Ŀ��:rolename
//
void drawrolename(wchar_t *str)
{
	rnxText.draw(str,rolenameposx,rolenameposy,rolenamewidth,rolenameoffset,rolenamecenter,rolenamecolor);
}

//==================================
//  ����:drawrtext(wchar_t *str,int x,int y,int width,int offsetH,bool center,D3DCOLOR color)
//  Ŀ��:�����ı�
//
void drawtext(wchar_t *str,int x,int y,int width,int offsetH,bool center,D3DCOLOR color)
{
	text.draw(str,x,y,width,offsetH,center,color);
}

//==================================
//  ����:getFPS()
//  Ŀ��:���FPSֵ
//
float getFPS()
{
	static float fps =0;
	static int   frameCount = 0;
	static float currentTime = 0.0f;
	static float lastTime = 0.0f;

	frameCount++;
	currentTime=timeGetTime()*0.001f;

	if(currentTime - lastTime > 1.0f)
	{
		fps = (float)frameCount / (currentTime - lastTime);
		lastTime = currentTime;
		frameCount = 0;
	}
	return fps;
}

void drawFPS()
{
	static wchar_t fpstext[15];
	swprintf(fpstext,15,L"FPSֵ: %0.0f",getFPS());
	fpsxText.draw(fpstext,0,0);
}