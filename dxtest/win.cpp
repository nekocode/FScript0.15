#include "win.h"

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	MsgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

INT WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPWSTR, INT )
{

	UNREFERENCED_PARAMETER(hInstance);

	// 注册窗口类
	MyRegisterClass(hInstance);
	// 创建主窗口
	if (!InitInstance (hInstance, SW_SHOWDEFAULT))
	{
		return FALSE;
	}

	//初始化D3D
	Init();

	// 主消息循环:
	MSG msg;
	ZeroMemory(&msg,sizeof(msg));
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg,NULL,0U,0U,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			gameMain();
	}

	Cleanup();
	
	UnregisterClass(szWindowClass,hinst);
	return 0;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_CLASSDC;
	wcex.lpfnWndProc	= MsgProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;//LoadIcon(NULL, IDI_APPLICATION);或者LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DXDEMO1))
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);         //背景颜色:黑色
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hinst = hInstance; // 将实例句柄存储在全局变量中

	RECT rect;
	rect.left=0;
	rect.top=0;
	rect.right=800;
	rect.bottom=600;
	//依据所需客户矩形的大小，计算需要的窗口矩形的大小(false=没有菜单)
	AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,false);

	hWnd = CreateWindowEx( NULL,
		szWindowClass,                     //窗口类名
		szTitle,                           //窗口标题
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,                     //初始x位置
		CW_USEDEFAULT,                     //初始y位置
		rect.right-rect.left,                               //初始宽度
		rect.bottom-rect.top,                               //初始高度
		NULL,
		NULL,                              //窗口菜单句柄
		hInstance,                         //实例句柄
		NULL );

	if (!hWnd)  
		return FALSE;

	hwnd=hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//获得exe所在目录
	wchar_t szbufPath[MAX_PATH];
	wchar_t CurrentPath[MAX_PATH];
	GetModuleFileName(NULL,CurrentPath,MAX_PATH);
	GetLongPathName(szbufPath,CurrentPath,MAX_PATH);
	for(int i=wcslen(CurrentPath);i>=0;i--)
	{
		if(CurrentPath[i]=='\\')
		{
			CurrentPath[i+1]='\0';
			break;
		}
	}
	//设置exe所在目录为当前目录
	SetCurrentDirectory(CurrentPath);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		RefVideo();
		GetClientRect(hwnd,&ClientRect);
		break;
/*	case WM_MOVE:
		break;*/
	case WM_MOUSEMOVE:
		mouse_x = (int)LOWORD(lParam);//取低位
        mouse_y = (int)HIWORD(lParam);//取高位
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}