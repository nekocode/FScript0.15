#include "dxinput.h"

IDirectInput8 *dinput;
IDirectInputDevice8 *dinputDM;          //MouseDevice
IDirectInputDevice8 *dinputDK;          //KeyboardDevice
DIDEVICEOBJECTDATA dinputMData[16];

extern int mouse_x,mouse_y;
extern RECT ClientRect;

struct MouseState
{
    LONG lAxisX;
    LONG lAxisY;
    BYTE abButtons[3];
    BYTE bPadding;       // Structure must be DWORD multiple in size.   
};
DIOBJECTDATAFORMAT g_aObjectFormats[] =
{
    { &GUID_XAxis, FIELD_OFFSET( MouseState, lAxisX ),    // X axis
        DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_YAxis, FIELD_OFFSET( MouseState, lAxisY ),    // Y axis
        DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { 0, FIELD_OFFSET( MouseState, abButtons[0] ),        // Button 0
        DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { 0, FIELD_OFFSET( MouseState, abButtons[1] ),        // Button 1 (optional)
        DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0 },
    { 0, FIELD_OFFSET( MouseState, abButtons[2] ),        // Button 2 (optional)
        DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0 }
};
#define numMouseObjects (sizeof(g_aObjectFormats) / sizeof(DIOBJECTDATAFORMAT))

DIDATAFORMAT            g_dfMouse =
{
    sizeof( DIDATAFORMAT ),
    sizeof( DIOBJECTDATAFORMAT ),
    DIDF_ABSAXIS,
    sizeof( MouseState ),
    numMouseObjects,
    g_aObjectFormats
};

MouseState ms;   //储存鼠标按键状态
char ks[256];    //储存键盘按键状态

//=================================================================================
//=================================================================================
//=================================================================================
//=================================================================================

bool dxInputInit()
{
	if(FAILED(DirectInput8Create(hinst,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&dinput,NULL)))
		return false;

	//初始化mouse对象
	if(FAILED(dinput->CreateDevice(GUID_SysMouse,&dinputDM,NULL)))
		return false;
	if(FAILED(dinputDM->SetCooperativeLevel(hwnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))    //窗口活动下获取&&非独占
		return false;
	if(FAILED(dinputDM->SetDataFormat(&g_dfMouse)))
		return false;
	dinputDM->Acquire();

	//初始化keyboaard对象
	if(FAILED(dinput->CreateDevice(GUID_SysKeyboard,&dinputDK,NULL)))
		return false;
	if(FAILED(dinputDK->SetCooperativeLevel(hwnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))    //窗口活动下获取&&非独占
		return false;
	if(FAILED(dinputDK->SetDataFormat(&c_dfDIKeyboard)))
		return false;
	dinputDK->Acquire();


	return true;
}

void DIRelease()
{
	dinputDM->Unacquire();
	dinputDK->Unacquire();

    if( dinputDM != NULL )
        dinputDM->Release();
	if( dinputDK != NULL )
        dinputDK->Release();

    if( dinput != NULL )
        dinput->Release();
}


void GetMousePos(int &x,int &y)
{
/*	POINT pos;
	
	//GetClientRect(hwnd,
	GetCursorPos(&pos);   //用windows函数获取鼠标坐标
	ScreenToClient(hwnd,&pos);  //转换为客户坐标
	
	x=pos.x*800/ClientRect.right;
	if(x<0) x=0;
	if(x>800) 
		x=800;

	y=pos.y*600/ClientRect.bottom;
	if(y<0) y=0;
	if(y>600) 
		y=600;*/
	if(ClientRect.right!=0&&ClientRect.bottom!=0)
	{
		x = mouse_x*800/ClientRect.right;
		y = mouse_y*600/ClientRect.bottom;
	}
}

bool MouseIsDown(int mKey)
{
	HRESULT hr;

	hr = dinputDM->Poll();
	if( FAILED( hr ) )
	{
		hr = dinputDM->Acquire();
		while( hr == DIERR_INPUTLOST )
			hr = dinputDM->Acquire();
//		return S_OK;
	}

	dinputDM->GetDeviceState( sizeof( MouseState ), &ms );
	 
	if( ms.abButtons[mKey] & 0x80)
		return true;

	 return false;
}

bool MouseIsClick(int mKey)
{
	static int olds[3];       //旧状态
	if((olds[mKey]==0))
	{
		if(MouseIsDown(mKey))
		{
			olds[mKey]=1;
			return true;
		}
		else
			return false;
	}
	else
	{
		if(!MouseIsDown(mKey))
			olds[mKey]=0;
		return false;
	}
}

bool KeyIsDown(int iKey)
{
	HRESULT hr;

	hr = dinputDK->Poll();
    if( FAILED( hr ) )
    {
        hr = dinputDK->Acquire();
        while( hr == DIERR_INPUTLOST )
            hr = dinputDK->Acquire();
        return S_OK;
    }

	dinputDK->GetDeviceState( sizeof(ks), (LPVOID)ks );
	 
	if( ks[iKey] & 0x80)
		return true;
	
	return false;
}

bool KeyIsClick(int iKey)
{
	static int olds[256];       //旧状态
	if((olds[iKey]==0))
	{
		if(KeyIsDown(iKey))
		{
			olds[iKey]=1;
			return true;
		}
		else
			return false;
	}
	else
	{
		if(!KeyIsDown(iKey))
			olds[iKey]=0;
		return false;
	}
}