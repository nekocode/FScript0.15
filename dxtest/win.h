#include <Windows.h>
#include <tchar.h>
#include "gameMain.h"


//ȫ�ֱ���
HINSTANCE hinst;								// ��ǰʵ��
HWND hwnd;                              // ��ǰ���ھ��
RECT ClientRect;                            //�ͻ�����ǰ����
int mouse_x,mouse_y;


LPCWSTR szTitle=L"fs0.15beta";					// �������ı�
LPCWSTR szWindowClass=L"fs";		    	// ����������