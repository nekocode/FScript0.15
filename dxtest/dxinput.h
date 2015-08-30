#include <dinput.h>
#include <Windows.h>

extern HINSTANCE hinst;				  //当前实例
extern HWND hwnd;                     //当前窗口句柄

bool dxInputInit();
void DIRelease();
void GetMousePos(int &,int &);
bool MouseIsDown(int);
bool MouseIsClick(int);
//mKey:0-左键 1-右键 2-中键
bool KeyIsDown(int);
bool KeyIsClick(int);