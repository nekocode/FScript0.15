#include <d3dx9core.h>
#include "FreeTypeLib.h";

#define DefaultBufLen 256
#define FontLibName "default.ttf"

bool initText(bool fisrt=true);
void ReleaseText();

void echo(wchar_t *);
void drawrolename(wchar_t *);
void drawtext(wchar_t *str,int x,int y,int widthh=800,int offsetH=0,bool center=false,D3DCOLOR color=0x00ffffff);
void drawFPS();


class dxtext
{
public:
	bool init(int height=20,int width=20);
	void draw(wchar_t *string,int x,int y,int width=800,int offsetH=0,bool center=false,D3DCOLOR color=0x00ffffff);
private:
	CFreeTypeLib* g_pFreeTypeLib;
};