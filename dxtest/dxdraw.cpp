#include "dxdraw.h"

#define valBuffer 195

LPDIRECT3D9 g_pD3D = NULL;              //Direct3D����
IDirect3DDevice9* g_pd3dD;              //Direct3D�豸����

extern int stop;
extern bool qpass;

dximg echoPic,rolenamePic,backPic,rolePic,rolePic2,roletemp;
bool echoOn=false,rolenamePicOn=false,backPicOn=false,rolePicOn=false,rolePic2On=false;
bool fpsOn=false;
float echoalpha = 0,backgroundalpha = 0,rolealpha = 0;

wchar_t echotext[valBuffer];           //Ҫecho���ı�
wchar_t rolename[10];                  //��ɫ��

float backfadestep=0;                  //�����������벽��
bool moveback=false;
float mbxstep,mbystep,mbcount;

extern int btnHeight;
extern float Top_Y;
extern btnlist *bl1;

//==================================
//  ����:Render()
//  Ŀ��:��Ⱦͼ�β���ʾ
//
void Render()
{
    if( NULL == g_pd3dD )
        return;

	//��պ�̨����
	g_pd3dD->Clear(0,NULL,D3DCLEAR_TARGET && D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,0),1.0f,0);

	if(SUCCEEDED(g_pd3dD->BeginScene()))
	{
		//�ں�̨�������ͼ��
		graphicRender();
		textRender();
		btnRender();

		//�����ں�̨������Ⱦͼ��
		g_pd3dD->EndScene();
	}
	//���ں�̨������Ƶ�ͼ���ύ��ǰ̨������ʾ
	g_pd3dD->Present(NULL,NULL,NULL,NULL);
}

//==================================
//  ����:graphicRender()
//  Ŀ��:ͼ����Ⱦ
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
		if(stop==3)        //��������״̬
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
		else if(stop==-3)   //��������״̬
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
		else if(stop==4)      //����ת��״̬
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

	if(stop==5)            //role����״̬
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
			stop = 0;    //��Ϊ��ȡָ��״̬
	}
	else if(stop==-5)      //role����״̬
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
			stop = 0;    //��Ϊ��ȡָ��״̬
		}
	}
	else if(stop==6)      //roleת��״̬
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
	else if(stop==-7)      //role��������
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
			stop = 0;    //��Ϊ��ȡָ��״̬
		}
	}
	else if(stop==7)        //role��������
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
			stop = 0;    //��Ϊ��ȡָ��״̬
		}
	}
	else                       //����״̬
	{
		if(rolePicOn)
			rolePic.draw();
		if(rolePic2On)
			rolePic2.draw();
	}

	//======================================================



	if(stop==2)            //echo����״̬
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
				stop=1;    //��Ϊ�ȴ�����״̬
			}
		}
	}
	else if(stop==-2)            //echo����״̬
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
				stop=0;    //��Ϊ��ȡָ��״̬
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
//  ����:textRender()
//  Ŀ��:�ı���Ⱦ
//
void textRender()
{
	/*/��ʾxy����
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
//  ����:btnRender()
//  Ŀ��:��ť��Ⱦ
//
void btnRender()
{
	btnlist *pos = bl1;
	int pos_X,pos_Y;
	//ÿ��btn��x,y����
	float offset_X,offset_Y;

	//����¼�
	btn_event();

	offset_Y = Top_Y;

	while(pos)
	{
		offset_X = pos->pos_x;

		//������Ļ��Χ�򲻻�ͼ
		if((pos->now_y > 601)||((pos->now_y+ pos->btn.height) < -1))
			pos->showOn = false;
		else
		{
			//���������������Ļ�������ˮƽ��λ��
			if(!(pos->inCls))
			{
				int center_Y = pos->now_y + pos->btn.height/2;
				//�������Ļ�ϰ벿�ֵĻ�
				if(center_Y < 300)
					offset_X = 800.0 - pos->btn.width*0.6 - (center_Y/300.0)*pos->btn.width*0.4;
				else
					offset_X = 800.0 - pos->btn.width*0.6 - (1.0-((center_Y-300.0)/300.0))*pos->btn.width*0.4;
			}
			pos->showOn = true;
		}

		//������Ѿ�������Ļ�����ͷ�
		if((pos->now_x < -1)||(pos->now_x > 801))
		{
			btnlist *dtmp;
			dtmp = pos;
			pos = pos->next;
			btnHeight = btnHeight - dtmp->btn.height;

			//�ͷŸ�btn
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
		//������Ѱ��һ��;
		pos = pos->next;
	}
}

//==================================
//  ����:setupViewAndProjMat()
//  Ŀ��:��ʼ��View��ProjMat����
//
void setupViewAndProjMat()
{
	//���ù۲����
    D3DXMATRIXA16 matView;
	D3DXMatrixIdentity( &matView );
    g_pd3dD->SetTransform( D3DTS_VIEW, &matView );

	//RECT WindowRect;
	//GetClientRect(hwnd, &WindowRect);

	//����ͶӰ����
	D3DXMATRIXA16 matProj;
	D3DXMatrixOrthoOffCenterLH( &matProj, 0.0f, 800.0f, 600.0f, 0.0f, 0.0f, 1.0f );
	g_pd3dD->SetTransform( D3DTS_PROJECTION, &matProj );
}

//==================================
//  ����:InitD3D( HWND )
//  Ŀ��:��ʼ��Direct3D�豸����
//
bool InitD3D()
{
	//����Direct3D����    [���ڻ�ȡӲ����Ϣ]
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_pD3D==NULL) return false;

	//����Ƿ�֧��Ӳ�����㴦��
	D3DCAPS9 caps;
	g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	int vp=0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp=D3DCREATE_HARDWARE_VERTEXPROCESSING;  //Ӳ������
	else
		vp=D3DCREATE_SOFTWARE_VERTEXPROCESSING;  //�������

	//���Ӳ��֧�ֵ�shader�İ汾
/*	if(caps.VertexShaderVersion < D3DVS_VERSION(2,0))
		deviceType = D3DDEVTYPE_REF;
	else
		deviceType = D3DDEVTYPE_HAL;*/

	//����D3DPRESENT_PARAMETERS�ṹ
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
//	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;  //ʵʱģʽ,Ĭ���Ǹ�����Ļˢ��Ƶ��

	//����Direct3D�豸����     [������Ⱦͼ��]
	if( FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,vp,&d3dpp,&g_pd3dD)) )
		return false;

	//���ù۲��ͶӰ����
	setupViewAndProjMat();

	//�������Թ���
	g_pd3dD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	g_pd3dD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	g_pd3dD->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	g_pd3dD->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);

	//��������Ѱַģʽ
	g_pd3dD->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x00000000);
	g_pd3dD->SetSamplerState(0, D3DSAMP_ADDRESSU,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(0, D3DSAMP_ADDRESSV,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(1, D3DSAMP_BORDERCOLOR, 0x00000000);
	g_pd3dD->SetSamplerState(1, D3DSAMP_ADDRESSU,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(1, D3DSAMP_ADDRESSV,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(2, D3DSAMP_BORDERCOLOR, 0x00000000);
	g_pd3dD->SetSamplerState(2, D3DSAMP_ADDRESSU,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(2, D3DSAMP_ADDRESSV,    D3DTADDRESS_BORDER);

	//�����涼����
	g_pd3dD->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    //�رչ���
	g_pd3dD->SetRenderState( D3DRS_LIGHTING, FALSE );

	//����alpha���
	g_pd3dD->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dD->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	InitVertex();      //��ʼ�����㻺��,����װ����ͼ

	return true;
}

//==================================
//  ����:D3DRelease()
//  Ŀ��:�ͷ�Direct3D��Direct3D�豸����
//
VOID D3DRelease()
{
	ReleaseVertex();

    if( g_pd3dD != NULL )
        g_pd3dD->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}