#include "dxdraw.h"
#include "dxinput.h"
#include "file.h"
#include "musicplay.h"
#include "video.h"
#include <Windows.h>

#include <gdiplus.h>
using namespace Gdiplus;


//extern long echoTime;                 //echo�ٶ�
extern HINSTANCE hinst;				  //��ǰʵ��
extern HWND hwnd;                     //��ǰ���ھ��
extern bool echoNew;

void gameMain();
void Init();
void Cleanup();
void roleeffect();
void add_event();
void do_event();