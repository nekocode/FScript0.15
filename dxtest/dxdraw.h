#include <Windows.h>
#include <tchar.h>
#include <d3d9.h>
#include "textdraw.h"
#include "dxinput.h"
#include "dxImage.h"
#include "butn.h"

extern HWND hwnd;

//�Զ��庯������
bool InitD3D();
void D3DRelease();
void Render();
void textRender();
void graphicRender();
void btnRender();