#include <dinput.h>
#include <Windows.h>

extern HINSTANCE hinst;				  //��ǰʵ��
extern HWND hwnd;                     //��ǰ���ھ��

bool dxInputInit();
void DIRelease();
void GetMousePos(int &,int &);
bool MouseIsDown(int);
bool MouseIsClick(int);
//mKey:0-��� 1-�Ҽ� 2-�м�
bool KeyIsDown(int);
bool KeyIsClick(int);