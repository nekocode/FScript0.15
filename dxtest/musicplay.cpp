#include "musicplay.h"
ZPlay *player[10];
TStreamStatus status;

void playmusic(char* file,int index)
{
	//�ж�����������ڲ����򵭳�
/*	player[index]->GetStatus(&status);
	if(status.fPlay)
	{

	}*/

	//���ļ�������
	player[index]->OpenFile(file, sfAutodetect);
	player[index]->Play();
}

void stopmusic(int index)
{
/*	TStreamTime start;
	TStreamTime end;
	unsigned int lv,rv;

	player[index]->GetPosition(&start);		
	end.sec = start.sec + 1;
	player[index]->SlideVolume( tfSecond, &start, 100,100, tfSecond, &end, 0,0);

	//����������δ����ʱ�ȴ�
	do
	{
		player[index]->GetPlayerVolume(&lv,&rv);
	}
	while(lv!=0);*/

	player[index]->Stop();

//	player[index]->SetPlayerVolume(100,100);
}

void initmusic()
{
	player[0] = CreateZPlay();
	player[1] = CreateZPlay();
	player[2] = CreateZPlay();
	player[3] = CreateZPlay();
	player[4] = CreateZPlay();
	player[5] = CreateZPlay();
	player[6] = CreateZPlay();
	player[7] = CreateZPlay();
	player[8] = CreateZPlay();
	player[9] = CreateZPlay();
}

void relmusic()
{
	player[0]->Release();
	player[1]->Release();
	player[2]->Release();
	player[3]->Release();
	player[4]->Release();
	player[5]->Release();
	player[6]->Release();
	player[7]->Release();
	player[8]->Release();
	player[9]->Release();
}


//��������
void fade(int index)
{
	TStreamTime start;
	TStreamTime end;
	unsigned int lv,rv;
		
	player[index]->GetPosition(&start);
	end.sec = start.sec + 1;
	player[index]->SlideVolume( tfSecond, &start, 100,100, tfSecond, &end, 0,0);

	//����������δ����ʱ�ȴ�
	do
	{
		player[index]->GetPlayerVolume(&lv,&rv);
	}
	while(lv!=0);
	player[index]->Stop();
	player[index]->SetPlayerVolume(100,100);
}