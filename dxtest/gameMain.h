#include "dxdraw.h"
#include "dxinput.h"
#include "file.h"
#include "musicplay.h"
#include "video.h"
#include <Windows.h>

#include <gdiplus.h>
using namespace Gdiplus;


//extern long echoTime;                 //echo速度
extern HINSTANCE hinst;				  //当前实例
extern HWND hwnd;                     //当前窗口句柄
extern bool echoNew;

void gameMain();
void Init();
void Cleanup();
void roleeffect();
void add_event();
void do_event();