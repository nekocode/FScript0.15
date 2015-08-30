#include "dxdraw.h"

#define valBuffer 195

LPDIRECT3D9 g_pD3D = NULL;              //Direct3D对象
IDirect3DDevice9* g_pd3dD;              //Direct3D设备对象

extern int stop;
extern bool qpass;

dximg echoPic,rolenamePic,backPic,rolePic,rolePic2,roletemp;
bool echoOn=false,rolenamePicOn=false,backPicOn=false,rolePicOn=false,rolePic2On=false;
bool fpsOn=false;
float echoalpha = 0,backgroundalpha = 0,rolealpha = 0;

wchar_t echotext[valBuffer];           //要echo的文本
wchar_t rolename[10];                  //角色名

float backfadestep=0;                  //背景淡出淡入步长
bool moveback=false;
float mbxstep,mbystep,mbcount;

extern int btnHeight;
extern float Top_Y;
extern btnlist *bl1;

//==================================
//  函数:Render()
//  目的:渲染图形并显示
//
void Render()
{
    if( NULL == g_pd3dD )
        return;

	//清空后台缓存
	g_pd3dD->Clear(0,NULL,D3DCLEAR_TARGET && D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,0),1.0f,0);

	if(SUCCEEDED(g_pd3dD->BeginScene()))
	{
		//在后台缓存绘制图形
		graphicRender();
		textRender();
		btnRender();

		//结束在后台缓存渲染图形
		g_pd3dD->EndScene();
	}
	//将在后台缓存绘制的图形提交到前台缓存显示
	g_pd3dD->Present(NULL,NULL,NULL,NULL);
}

//==================================
//  函数:graphicRender()
//  目的:图像渲染
//
void graphicRender()
{
	static float echoalpha = 0,backgroundalpha = 0,rolealpha = 0;
	static int mbi=0;

	if(moveback)
	{
		if(qpass)
		{
			mbi = mbcount;
			qpass = false;
		}
		else
			mbi++;

		backPic.move(mbxstep,mbystep);
		if(mbi==(int)mbcount)
		{
			mbi=0;
			moveback=false;
		}
	}

	if(backPicOn)
	{
		if(stop==3)        //背景淡入状态
		{
			if(qpass)
			{
				backgroundalpha = 255;
				qpass = false;
			}

			backPic.draw(backgroundalpha);
			if(fadein(backgroundalpha,backfadestep))
				stop = 0;
		}
		else if(stop==-3)   //背景淡出状态
		{
			if(qpass)
			{
				backgroundalpha = 0;
				qpass = false;
			}

			backPic.draw(backgroundalpha);
			if(fadeout(backgroundalpha,backfadestep))
			{
				backPicOn = false;
				echoalpha=0;
				rolealpha=0;
				mbi=0;
				moveback=false;
				stop = 0;
			}
		}
		else if(stop==4)      //背景转换状态
		{
			if(qpass)
			{
				backgroundalpha = 0;
				qpass = false;
			}

			backPic.draw(backgroundalpha);
			if(fadeout(backgroundalpha,backfadestep))
			{
				backgroundalpha = 255;
				echoalpha=0;
				stop = 0;
			}
		}
		else
			backPic.draw();
	}
	//======================================================

	if(stop==5)            //role淡入状态
	{
		if(qpass)
		{
			rolealpha = 255;
			qpass = false;
		}

		if(rolePicOn)
			rolePic.draw(rolealpha);
		if(rolePic2On)
			rolePic2.draw(rolealpha);
		if(fadein(rolealpha))
			stop = 0;    //变为读取指令状态
	}
	else if(stop==-5)      //role淡出状态
	{
		if(qpass)
		{
			rolealpha = 0;
			qpass = false;
		}

		if(rolePicOn)
			rolePic.draw(rolealpha);
		else
		{
			rolealpha=0;
			stop=0;
		}
		if(rolePic2On)
			rolePic2.draw(rolealpha);

		if(fadeout(rolealpha))
		{
			rolePicOn = false;
			rolePic2On = false;
			stop = 0;    //变为读取指令状态
		}
	}
	else if(stop==6)      //role转换状态
	{
		if(qpass)
		{
			rolealpha = 0;
			qpass = false;
		}

		if(rolePicOn)
			rolePic.draw(rolealpha);
		if(rolePic2On)
			rolePic2.draw(rolealpha);
		if(fadeout(rolealpha))
		{
			rolealpha = 255;
			stop = 0;
		}
	}
	else if(stop==-7)      //role数量减少
	{
		if(qpass)
		{
			rolealpha = 0;
			qpass = false;
		}

		roletemp.draw(255-rolealpha);
		rolePic.draw(rolealpha);
		rolePic2.draw(rolealpha);
		if(fadeout(rolealpha))
		{
			changeImg(&rolePic,&roletemp);

			rolePic2On = false;
			rolealpha = 255;
			stop = 0;    //变为读取指令状态
		}
	}
	else if(stop==7)        //role数量增多
	{
		if(qpass)
		{
			rolealpha = 0;
			qpass = false;
		}

		rolePic.draw(rolealpha);
		rolePic2.draw(255-rolealpha);
		roletemp.draw(255-rolealpha);
		if(fadeout(rolealpha))
		{
			changeImg(&rolePic,&roletemp);

			rolealpha = 255;
			stop = 0;    //变为读取指令状态
		}
	}
	else                       //常规状态
	{
		if(rolePicOn)
			rolePic.draw();
		if(rolePic2On)
			rolePic2.draw();
	}

	//======================================================



	if(stop==2)            //echo淡入状态
	{
		if(qpass)
		{
			echoalpha = 255;
			qpass = false;
		}

		if(echoOn)
		{
			echoPic.draw(echoalpha);
			if(fadein(echoalpha))
			{
				stop=1;    //变为等待按键状态
			}
		}
	}
	else if(stop==-2)            //echo淡出状态
	{
		if(qpass)
		{
			echoalpha = 0;
			qpass = false;
		}

		if(echoOn)
		{
			echoPic.draw(echoalpha);
			if(fadeout(echoalpha))
			{
				echoOn = false;
				stop=0;    //变为读取指令状态
			}
		}
		else
		{
			echoalpha=0;
			stop=0;
		}
	}
	else
	{
		if(echoOn)
			echoPic.draw();
	}




	if(rolenamePicOn&&(stop!=2&&stop!=-2))
		rolenamePic.draw();
}

//==================================
//  函数:textRender()
//  目的:文本渲染
//
void textRender()
{
	/*/显示xy坐标
	int x,y;
	wchar_t xt[15],yt[15];
	GetMousePos(x,y);

	swprintf(xt,15,L"X: %d",x);
	drawtext(xt,0,50);

	swprintf(yt,15,L"Y: %d",y);
	drawtext(yt,0,70);//*/


	if(fpsOn)
		drawFPS();

	if(echoOn&&(stop!=2&&stop!=-2&&stop!=3&&stop!=-3&&stop!=4))
		echo(echotext);

	if(rolenamePicOn&&(stop!=2&&stop!=-2&&stop!=3&&stop!=-3&&stop!=4))
		drawrolename(rolename);
}

//==================================
//  函数:btnRender()
//  目的:按钮渲染
//
void btnRender()
{
	btnlist *pos = bl1;
	int pos_X,pos_Y;
	//每个btn的x,y坐标
	float offset_X,offset_Y;

	//鼠标事件
	btn_event();

	offset_Y = Top_Y;

	while(pos)
	{
		offset_X = pos->pos_x;

		//超出屏幕范围则不绘图
		if((pos->now_y > 601)||((pos->now_y+ pos->btn.height) < -1))
			pos->showOn = false;
		else
		{
			//如果不是正在清除的话则设置水平轴位置
			if(!(pos->inCls))
			{
				int center_Y = pos->now_y + pos->btn.height/2;
				//如果在屏幕上半部分的话
				if(center_Y < 300)
					offset_X = 800.0 - pos->btn.width*0.6 - (center_Y/300.0)*pos->btn.width*0.4;
				else
					offset_X = 800.0 - pos->btn.width*0.6 - (1.0-((center_Y-300.0)/300.0))*pos->btn.width*0.4;
			}
			pos->showOn = true;
		}

		//如果有已经缩出屏幕的则释放
		if((pos->now_x < -1)||(pos->now_x > 801))
		{
			btnlist *dtmp;
			dtmp = pos;
			pos = pos->next;
			btnHeight = btnHeight - dtmp->btn.height;

			//释放该btn
			if(dtmp!=bl1)
				dtmp->front->next = dtmp->next;
			else
				bl1=NULL;

			if(dtmp->next)
				dtmp->next->front = dtmp->front;
			delete dtmp;
			dtmp = 0;

			continue;
		}

		pos->setnextpos(offset_X,offset_Y);

		pos->drawbtn();
		if(pos->text[0]!='\0')
			drawtext(pos->text,pos->now_x + pos->btn.width/6,pos->now_y + pos->btn.height/2 - 10,pos->btn.width,0,false,0x00000000);

		offset_Y = offset_Y + pos->btn.height;
		//继续搜寻下一个;
		pos = pos->next;
	}
}

//==================================
//  函数:setupViewAndProjMat()
//  目的:初始化View和ProjMat矩阵
//
void setupViewAndProjMat()
{
	//设置观察矩阵
    D3DXMATRIXA16 matView;
	D3DXMatrixIdentity( &matView );
    g_pd3dD->SetTransform( D3DTS_VIEW, &matView );

	//RECT WindowRect;
	//GetClientRect(hwnd, &WindowRect);

	//设置投影矩阵
	D3DXMATRIXA16 matProj;
	D3DXMatrixOrthoOffCenterLH( &matProj, 0.0f, 800.0f, 600.0f, 0.0f, 0.0f, 1.0f );
	g_pd3dD->SetTransform( D3DTS_PROJECTION, &matProj );
}

//==================================
//  函数:InitD3D( HWND )
//  目的:初始化Direct3D设备对象
//
bool InitD3D()
{
	//创建Direct3D对象    [用于获取硬件信息]
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_pD3D==NULL) return false;

	//检查是否支持硬件顶点处理
	D3DCAPS9 caps;
	g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	int vp=0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp=D3DCREATE_HARDWARE_VERTEXPROCESSING;  //硬件处理
	else
		vp=D3DCREATE_SOFTWARE_VERTEXPROCESSING;  //软件处理

	//检查硬件支持的shader的版本
/*	if(caps.VertexShaderVersion < D3DVS_VERSION(2,0))
		deviceType = D3DDEVTYPE_REF;
	else
		deviceType = D3DDEVTYPE_HAL;*/

	//设置D3DPRESENT_PARAMETERS结构
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
//	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;  //实时模式,默认是根据屏幕刷新频率

	//创建Direct3D设备对象     [用于渲染图形]
	if( FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,vp,&d3dpp,&g_pd3dD)) )
		return false;

	//设置观察和投影矩阵
	setupViewAndProjMat();

	//设置线性过滤
	g_pd3dD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	g_pd3dD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	g_pd3dD->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	g_pd3dD->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);

	//设置纹理寻址模式
	g_pd3dD->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x00000000);
	g_pd3dD->SetSamplerState(0, D3DSAMP_ADDRESSU,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(0, D3DSAMP_ADDRESSV,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(1, D3DSAMP_BORDERCOLOR, 0x00000000);
	g_pd3dD->SetSamplerState(1, D3DSAMP_ADDRESSU,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(1, D3DSAMP_ADDRESSV,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(2, D3DSAMP_BORDERCOLOR, 0x00000000);
	g_pd3dD->SetSamplerState(2, D3DSAMP_ADDRESSU,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(2, D3DSAMP_ADDRESSV,    D3DTADDRESS_BORDER);

	//正反面都绘制
	g_pd3dD->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    //关闭光照
	g_pd3dD->SetRenderState( D3DRS_LIGHTING, FALSE );

	//激活alpha混合
	g_pd3dD->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dD->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	InitVertex();      //初始化顶点缓存,用于装载贴图

	return true;
}

//==================================
//  函数:D3DRelease()
//  目的:释放Direct3D及Direct3D设备对象
//
VOID D3DRelease()
{
	ReleaseVertex();

    if( g_pd3dD != NULL )
        g_pd3dD->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}