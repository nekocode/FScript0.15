#include "win.h"

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	MsgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

INT WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPWSTR, INT )
{

	UNREFERENCED_PARAMETER(hInstance);

	// ע�ᴰ����
	MyRegisterClass(hInstance);
	// ����������
	if (!InitInstance (hInstance, SW_SHOWDEFAULT))
	{
		return FALSE;
	}

	//��ʼ��D3D
	Init();

	// ����Ϣѭ��:
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
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
	wcex.hIcon			= NULL;//LoadIcon(NULL, IDI_APPLICATION);����LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DXDEMO1))
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);         //������ɫ:��ɫ
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hinst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	RECT rect;
	rect.left=0;
	rect.top=0;
	rect.right=800;
	rect.bottom=600;
	//��������ͻ����εĴ�С��������Ҫ�Ĵ��ھ��εĴ�С(false=û�в˵�)
	AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,false);

	hWnd = CreateWindowEx( NULL,
		szWindowClass,                     //��������
		szTitle,                           //���ڱ���
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,                     //��ʼxλ��
		CW_USEDEFAULT,                     //��ʼyλ��
		rect.right-rect.left,                               //��ʼ���
		rect.bottom-rect.top,                               //��ʼ�߶�
		NULL,
		NULL,                              //���ڲ˵����
		hInstance,                         //ʵ�����
		NULL );

	if (!hWnd)  
		return FALSE;

	hwnd=hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//���exe����Ŀ¼
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
	//����exe����Ŀ¼Ϊ��ǰĿ¼
	SetCurrentDirectory(CurrentPath);

	return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
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
		mouse_x = (int)LOWORD(lParam);//ȡ��λ
        mouse_y = (int)HIWORD(lParam);//ȡ��λ
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}