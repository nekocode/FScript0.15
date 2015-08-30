#include "gameMain.h"
#include "sleep.h"

ULONG_PTR m_gdiplusToken;//为全局变量

void DbcToSbc(wchar_t*,wchar_t*); //半角转全角

extern float backfadestep;             //背景淡出淡入步长
extern bool moveback;
extern float mbxstep,mbystep,mbcount;

extern bool echoruned;
extern bool echoNew;
extern bool echoAllShow;

extern cmd cmdread;
extern char strval[2][valBuffer];
extern bool mask[2];
extern int intval[5];
extern int valnum;
extern int intpath[3][4];

//根据debug状态,控制窗口是否置顶显示
#define DEBUG

//=============参数==================
extern int echosize,echoposx,echoposy,echowidth,echooffset,echocolor,rolenamesize,rolenameposx,rolenameposy,rolenamewidth,rolenameoffset,rolenamecolor;
extern bool echocenter,rolenamecenter;
//===================================

bool musicloop[3] = {true,true,true};
extern ZPlay *player[3];
extern TStreamStatus status;

int stop = 0;
//暂停标志: 0.读取命令(不暂停)
//         1.暂停等待按键         -1.sleep状态(等待按键)
//         2.echo淡入            -2.echo淡出
//         3.background淡入      -3.background淡出
//         4.background转变
//         5.role淡入            -5.role淡出
//         6.role转变             
//         7.role数量增多         -7.role数量减少
//         8.视频播放状态          9.脚本结束状态
//        10.菜单状态

//是否快进当前命令
bool qpass = false;

float hz = 0.06;          //屏幕刷新率/1000,默认为0.06
time sleeptime;           //用来控制sleep命令
int sleepspeed;           //sleep命令的参数

extern wchar_t echotext[valBuffer];
extern wchar_t rolename[10];
extern bool echoOn,rolenamePicOn,backPicOn,rolePicOn,rolePic2On;
extern bool fpsOn;
extern dximg echoPic,rolenamePic,backPic,rolePic,rolePic2,roletemp;
extern dximg btn;

//按钮列表
extern btnlist *bl1;
extern btnlist *bl1_pos;
//鼠标停留的btn序号
extern int over_index;
//鼠标按下的btn序号
extern int down_index;
//鼠标单击的btn序号
extern int click_index;

//==================================
//  函数:gameMain()
//  目的:游戏主过程,读取文件,分析脚本,渲染图形
//
void gameMain()
{
	do_event();

	if(stop==0)
	{
		read();

		if(cmdread==echoCmd)
		{
			//将要echo的文本传输给echotext(多字节转宽字节),后面并打开echoOn和echoNew
			mbstowcs(echotext,strval[0],valBuffer);
			//将echoText转换为全角
			DbcToSbc(echotext,echotext);

			if(echoOn==true)  //已经打开了echoOn的话则不淡入
				stop = 1;
			else
				stop = 2;


			if(valnum==2)
			{
				mbstowcs(rolename,strval[1],10);
				rolenamePicOn = true;                  //打开rolenamePicOn
			}
			else
				rolenamePicOn=false;

			echoOn = true;
			echoNew = true;
		}
		else if(cmdread==roleCmd)
		{
			roleeffect();                          //跳转到roleeffect处理
		}
		else if(cmdread==clearroleCmd)
		{
			stop = -5;        //淡出
		}
		else if(cmdread==clearechoCmd)
		{
			stop = -2;
			rolenamePicOn=false;
		}
		else if(cmdread==backgroundCmd)
		{
			wchar_t Path[valBuffer];
			mbstowcs(Path,strval[0],valBuffer);

			//关闭echo
			echoOn=false;
			rolenamePicOn = false;

			if(backPicOn==true)           //切换背景
			{
				backPic.loadnextimg(Path,mask[0]);
				backPic.setWH2(intpath[0][0],intpath[0][1]);
				backPic.setpos2(intpath[0][2],intpath[0][3]);

				moveback=false;
				stop=4;
			}
			else                          //淡入背景
			{
				backPic.loadimg(Path,mask[0]);
				backPic.setWH(intpath[0][0],intpath[0][1]);
				backPic.setpos(intpath[0][2],intpath[0][3]);

				backPicOn=true;
				stop=3;
			}
			if(intval[0]!=0)
				backfadestep = 256.0f/(intval[0]*hz);
			else
				backfadestep = 4.2f;
		}
		else if(cmdread==clearbackgroundCmd)
		{
			if(intval[0]!=0)
				backfadestep = 256.0f/(intval[0]*hz);
			else
				backfadestep = 4.2f;
			stop=-3;

			//同时清除掉其他元素
			rolePicOn=false;
			rolePic2On=false;
			rolenamePicOn=false;
			echoOn=false;
		}
		else if(cmdread==stopCmd)
		{
			stop = 1;
		}
		else if(cmdread==sleepCmd)
		{
			sleepspeed = intval[0];
			stop = -1;
		}
		else if(cmdread==playmusicCmd)
		{
			playmusic(strval[0]);
			if(intval[0]==1)
				musicloop[0] = false;
			else
				musicloop[0] = true;
		}
		else if(cmdread==stopmusicCmd)
		{
			musicloop[0] = false;
			stopmusic();
		}
		else if(cmdread==playmusic2Cmd)
		{
			playmusic(strval[0],1);
			if(intval[0]==1)
				musicloop[1] = false;
			else
				musicloop[1] = true;
		}
		else if(cmdread==stopmusic2Cmd)
		{
			musicloop[1] = false;
			stopmusic(1);
		}
		else if(cmdread==playmusic3Cmd)
		{
			playmusic(strval[0],2);
			if(intval[0]==1)
				musicloop[2] = false;
			else
				musicloop[2] = true;
		}
		else if(cmdread==movebackgroundCmd)
		{
			if(backPicOn)
			{
				intval[2];
				mbcount = intval[2]*hz;
				mbxstep = (backPic.nowx>intval[0]?-(backPic.nowx - intval[0]):(intval[0] - backPic.nowx)) / mbcount;
				mbystep = (backPic.nowy>intval[1]?-(backPic.nowy - intval[1]):(intval[1] - backPic.nowy)) / mbcount;
				moveback=true;
			}
		}
		else if(cmdread==btnCmd)
		{
			wchar_t text[valBuffer];
			wchar_t path[valBuffer];
			mbstowcs(text,strval[0],valBuffer);
			mbstowcs(path,strval[1],valBuffer);
			addBtn(intval[0],text,path);
		}
		else if(cmdread==clbtnCmd)
		{
			clsBtn(intval[0]);
		}
		else if(cmdread==eventCmd)
		{
			add_event();
		}
		//========================================================
		else if(cmdread==videoCmd)
		{
			wchar_t Path[valBuffer];
			mbstowcs(Path,strval[0],valBuffer);
			playVideo(Path);
			stop = 8;
		}
		else if(cmdread==echoposCmd)
		{
			echoposx=intval[0];
			echoposy=intval[1];
			echocenter=intval[2];
		}
		else if(cmdread==echocolorCmd)
		{
			echocolor = D3DCOLOR_RGBA(intval[0],intval[1],intval[2],255-intval[3]);
		}
		else if(cmdread==echowidthCmd)
		{
			echowidth=intval[0];
		}
		else if(cmdread==echofontsizeCmd)       //
		{
			echosize=intval[0];
			initText(false);
		}
		else if(cmdread==rolenameposCmd)
		{
			rolenameposx=intval[0];
			rolenameposy=intval[1];
			rolenamecenter=intval[2];
		}
		else if(cmdread==rolenamecolorCmd)
		{
			rolenamecolor = D3DCOLOR_RGBA(intval[0],intval[1],intval[2],255-intval[3]);
		}
		else if(cmdread==rolenamewidthCmd)
		{
			rolenamewidth=intval[0];
		}
		else if(cmdread==rolenamefontsizeCmd)   //
		{
			rolenamesize=intval[0];
			initText(false);
		}
		else if(cmdread==echopicCmd)
		{
			wchar_t Path[valBuffer];
			mbstowcs(Path,strval[0],valBuffer);
			echoPic.loadimg(Path);
			echoPic.setpos(intval[0],intval[1]);
		}
		else if(cmdread==rolenamepicCmd)
		{
			wchar_t Path[valBuffer];
			mbstowcs(Path,strval[0],valBuffer);
			rolenamePic.loadimg(Path);
			rolenamePic.setpos(intval[0],intval[1]);
		}
		else if(cmdread==fpsCmd)
		{
			fpsOn = true;
		}
		else if(cmdread==endCmd)
		{
			stop=9;
		}
	}

	//====================等待按键状态
	else if(stop==1)
	{
		if(MouseIsClick(0)||KeyIsClick(DIK_SPACE)||KeyIsClick(DIK_RETURN)||KeyIsClick(DIK_NUMPADENTER))
		{
			if(echoruned==true)
				echoAllShow = true;
			else
				stop = 0;
		}
	}
	//====================sleep状态
	else if(stop==-1)
	{
		if(sleeptime.sleep(sleepspeed))
			stop = 0;
	}
	//====================视频播放状态
	else if(stop==8)
	{
		if(valnum==2)
		{
			if(MouseIsClick(0)||KeyIsClick(DIK_SPACE))
			{
				stopVideo();
				stop = 0;
			}
		}

		if(VideoIsEnd())
		{
			stopVideo();
			stop=0;
		}
		return;

	}
	//====================渲染图形
	Render();
	//====================

	//控制音乐循环
	for(int i=0;i<3;i++)
	{
		if(musicloop[i])
		{
			player[i]->GetStatus(&status);
			if(!status.fPlay)
				player[i]->Play();
		}
	}
}

void Init()
{
#ifdef DEBUG
	SetWindowPos(hwnd,HWND_TOPMOST,1000,0,0,0,SWP_NOSIZE);
#endif

	//修改运行时字符集环境
	setlocale(LC_ALL, "chs");

	//初始化文件读取模块
	initFile();
	//获得文件的所有入口点
	getAllPos();

	if(!InitD3D())
		MessageBox(hwnd,TEXT("Direct3D init error!"),TEXT("Error"),MB_OK);
	if(!dxInputInit())
		MessageBox(hwnd,TEXT("DirectInput init error!"),TEXT("Error"),MB_OK);
	if(!InitEffect())
		MessageBox(hwnd,TEXT("Effect init error!"),TEXT("Error"),MB_OK);
	//初始化D3Dfont
	if(!initText())
		MessageBox(hwnd,TEXT("myText init error!"),TEXT("Error"),MB_OK);
//	if(!initVideo())
//		MessageBox(hwnd,TEXT("Video init error!"),TEXT("Error"),MB_OK);

	//获得屏幕刷新率
	DEVMODEW dev;
	EnumDisplaySettings(NULL,0,&dev);
	hz = float(dev.dmDisplayFrequency)*0.001f;

	initmusic();

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

}

void Cleanup()
{
	OffFile();
	ReleaseText();
	ReleaseEffect();
	releaseVideo();
	D3DRelease();
	DIRelease();
	relmusic();
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

//===========================
//半角转全角
//===========================
void DbcToSbc(wchar_t* stringText,wchar_t* destText)
{
	int nLength=_tcslen(stringText);

	wchar_t *c=new wchar_t[sizeof(wchar_t)*nLength+1];
	memset(c,0,sizeof(wchar_t)*nLength+1);
	wcscpy(c,stringText);
	for (int i=0;i<nLength;i++)
	{
		if (c[i]==32)
		{ 
			c[i]=(wchar_t)12288;
			continue;
		}
		if (c[i]==10)     //如果为换行符的话跳过
		{ 
			c[i]=10;
			continue;
		}
		if (c[i]<127)
		{
			c[i]=(wchar_t)(c[i]+65248);
		}
	}
	wcscpy(destText,c);
}

void roleeffect()
{
	wchar_t Path[valBuffer];
	mbstowcs(Path,strval[0],valBuffer);

	if(valnum==1)
	{
		if(rolePic2On==true)          //数量减少
		{
			roletemp.loadimg(Path,mask[0]);
			roletemp.setWH(intpath[0][0],intpath[0][1]);
			if(!intpath[0][2])
			{
				if(roletemp.height<600)
					intpath[0][2] = 600 - roletemp.height;
			}
			roletemp.setpos((400.0f-roletemp.width/2),intpath[0][2]);
			stop = -7;
		}
		else if(rolePicOn==true)     //转变
		{
			rolePic.loadnextimg(Path,mask[0]);
			rolePic.setWH2(intpath[0][0],intpath[0][1]);
			if(!intpath[0][2])
			{
				if(rolePic.height2<600)
					intpath[0][2] = 600 - rolePic.height2;
			}
			rolePic.setpos2((400.0f-rolePic.width2/2),intpath[0][2]);
			stop = 6;
		}
		else                        //淡入
		{
			rolePic.loadimg(Path,mask[0]);
			rolePic.setWH(intpath[0][0],intpath[0][1]);
			if(!intpath[0][2])
			{
				if(rolePic.height<600)
					intpath[0][2] = 600 - rolePic.height;
			}
			rolePic.setpos((400.0f-rolePic.width/2),intpath[0][2]);
			stop = 5;
		}
		rolePic2On = false;
	}


	else           //双立绘
	{
		if(rolePicOn==true&&rolePic2On==false)          //数量增多
		{
			roletemp.loadimg(Path,mask[0]);
			roletemp.setWH(intpath[0][0],intpath[0][1]);
			if(!intpath[0][2])
			{
				if(roletemp.height<600)
					intpath[0][2] = 600 - roletemp.height;
			}
			roletemp.setpos((230.0f-roletemp.width/2),intpath[0][2]);
			stop = 7;


			mbstowcs(Path,strval[1],valBuffer);
			rolePic2.loadimg(Path,mask[1]);
			rolePic2.setWH(intpath[1][0],intpath[1][1]);
			if(!intpath[1][2])
			{
				if(rolePic2.height<600)
					intpath[1][2] = 600 - rolePic2.height;
			}
			rolePic2.setpos((570.0f-rolePic2.width/2),intpath[1][2]);
		}
		else if(rolePicOn==true&&rolePic2On==true)   //转变
		{
			rolePic.loadnextimg(Path,mask[0]);
			rolePic.setWH2(intpath[0][0],intpath[0][1]);
			if(!intpath[0][2])
			{
				if(rolePic.height2<600)
					intpath[0][2] = 600 - rolePic.height2;
			}
			rolePic.setpos2((230.0f-rolePic.width2/2),intpath[0][2]);


			mbstowcs(Path,strval[1],valBuffer);
			rolePic2.loadnextimg(Path,mask[1]);
			rolePic2.setWH2(intpath[1][0],intpath[1][1]);
			if(!intpath[1][2])
			{
				if(rolePic2.height2<600)
					intpath[1][2] = 600 - rolePic2.height2;
			}
			rolePic2.setpos2((570.0f-rolePic2.width2/2),intpath[1][2]);
			stop = 6;
		}
		else                       //淡入
		{
			rolePic.loadimg(Path,mask[0]);
			rolePic.setWH(intpath[0][0],intpath[0][1]);
			if(!intpath[0][2])
			{
				if(rolePic.height<600)
					intpath[0][2] = 600 - rolePic.height;
			}
			rolePic.setpos((230.0f-rolePic.width/2),intpath[0][2]);

			mbstowcs(Path,strval[1],valBuffer);
			rolePic2.loadimg(Path,mask[1]);
			rolePic2.setWH(intpath[1][0],intpath[1][1]);
			if(!intpath[1][2])
			{
				if(rolePic2.height<600)
					intpath[1][2] = 600 - rolePic2.height;
			}
			rolePic2.setpos((570.0f-rolePic2.width/2),intpath[1][2]);
			stop = 5;
		}
		rolePic2On = true;
	}

	rolePicOn = true;
}

void add_event()
{
	//按钮事件
	if(strval[0][0]=='b'&&strval[0][1]=='t'&&strval[0][2]=='n'&&strval[0][3]=='_')
	{
		btnlist *pos = bl1;
		while(pos)
		{
			if(pos->index==intpath[0][0])
				break;
			pos = pos->next;
		}

		if(!pos)
			return;

		//按钮按下事件
		if(strval[0][4]=='c'&&strval[0][5]=='l'&&strval[0][6]=='i'&&strval[0][7]=='c'&&strval[0][8]=='k')
		{
			strcpy_s(pos->fucn,strval[1]);
		}
		//按钮over事件
		else if(strval[0][4]=='o'&&strval[0][5]=='v'&&strval[0][6]=='e'&&strval[0][7]=='r')
		{
		}
	}
	else if(strval[0][0]=='k'&&strval[0][1]=='e'&&strval[0][2]=='y'&&strval[0][3]=='_')
	{
	}
}

void do_event()
{
	//判断是否发生按钮事件
	if(click_index!=-1)
	{
		btnlist *pos = bl1;
		while(pos)
		{
			if(pos->index==click_index)
				break;
			pos = pos->next;
		}
		if(pos->fucn=="")
			return;

		if(callfucn(pos->fucn))
		{
			if(stop==10||stop==-1||stop==9)
				stop = 0;
			else
				//快进当前命令
				qpass = true;
		}
	}
	else
		qpass = false;
}