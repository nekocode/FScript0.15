#include "butn.h"

//btn�б�
btnlist *bl1;
btnlist *bl1_pos;

//��һ����ť��y����
float Top_Y;
//���а�ť�ĸ�
int btnHeight=0;

//���ͣ����btn���
int over_index=-1;

//��갴�µ�btn���
int down_index = -1;

//��굥����btn���
int click_index=-1;

//==================================
//  btnlist���ʵ�ֺ���
//  Ŀ��:�����ƶ�����,����btn
//
void btnlist::setnowpos(float x,float y)
{
	now_x = x;now_y = y;
	btn.setpos(x,y);
}

void btnlist::setnextpos(float x,float y)
{
	pos_x = x;pos_y = y;

	//��ȡx��y���·������
	float S_x = (x - now_x);
	float S_y = (y - now_y);

	//��ȡx��y��ĳ��ٶ�
	v0_x = S_x * 2 / btnTime;
	v0_y = S_y * 2 / btnTime;

	//��ȡx��y��ļ��ٶ�
	if(S_x==0)
		a_x = 0;
	else
		a_x = - v0_x * v0_x / (S_x * 2);

	if(S_y==0)
		a_y = 0;
	else
		a_y = - v0_y * v0_y / (S_y * 2);
}

void btnlist::drawbtn()
{
	//һ���ӵ�·��
	float s_x=0;
	float s_y=0;

	if(a_x<0)
	{
		if(v0_x>0)
		{
			s_x = v0_x + 0.5 * a_x;
			v0_x = v0_x + a_x;
		}
		else
			s_x = 0;
	}
	else
	{
		if(v0_x<0)
		{
			s_x = v0_x + 0.5 * a_x;
			v0_x = v0_x + a_x;
		}
		else
			s_x = 0;
	}

	if(a_y<0)
	{
		if(v0_y>0)
		{
			s_y = v0_y + 0.5 * a_y;
			v0_y = v0_y + a_y;
		}
		else
			s_y = 0;
	}
	else
	{
		if(v0_y<0)
		{
			s_y = v0_y + 0.5 * a_y;
			v0_y = v0_y + a_y;
		}
		else
			s_y = 0;
	}

	now_x = now_x+s_x;
	now_y = now_y+s_y;

	if(showOn)
	{
		btn.setpos(now_x ,now_y);
		btn.draw();
	}
}

void addBtn(int index,wchar_t* text,wchar_t* path)
{
	btnlist *bl_temp = new btnlist;
	float nowY=0;
	nowY = Top_Y;

	if(!bl1)
	{
		wcscpy_s(bl_temp->text,text);
		bl_temp->btn.loadimg(path);
		bl_temp->index = index;

		bl1 = bl_temp;
		bl1->next = NULL;
	}
	else
	{
		bl1_pos = bl1;

		while(bl1_pos)
		{
			if(bl1_pos->index > index)
			{
				wcscpy_s(bl_temp->text,text);
				bl_temp->btn.loadimg(path);
				bl_temp->index = index;

				//˫���б�,��ǰ��
				//������ڿ�ͷ��ǰ��Ļ�
				if(bl1_pos==bl1)
				{
					bl_temp->next = bl1;
					bl1->front = bl_temp;
					bl1 = bl_temp;
				}
				else
				{
					bl_temp->next = bl1_pos;
					bl_temp->front = bl1_pos->front;
					bl1_pos->front->next = bl_temp;
					bl1_pos->front = bl_temp;
				}
				break;
			}
			//����б������и�btn
			else if(bl1_pos->index == index)
			{
				delete bl_temp;
				wcscpy_s(bl1_pos->text,text);
				bl1_pos->btn.loadimg(path);

				return;
			}

			nowY = nowY + bl1_pos->btn.height;

			//���������һ��
			if(!bl1_pos->next)
			{
				wcscpy_s(bl_temp->text,text);
				bl_temp->btn.loadimg(path);
				bl_temp->index = index;

				bl_temp->front = bl1_pos;
				bl1_pos->next =bl_temp;
				bl_temp->next = NULL;

				break;
			}

			bl1_pos = bl1_pos->next;
		}
	}

	bl_temp->showOn = false;
	btnHeight = btnHeight + bl_temp->btn.height;


	if(bl1->next==NULL)
	{
		Top_Y = 300.0 - bl_temp->btn.height/2.0;

		bl_temp->inCls = false;
		bl_temp->setnowpos(800,Top_Y);
		bl_temp->setnextpos(800,Top_Y);
	}
	else
	{
		Top_Y = Top_Y - bl_temp->btn.height/2.0;
		bl_temp->inCls = false;
		bl_temp->setnowpos(800,nowY);

		if((nowY < 601)&&((nowY + bl_temp->btn.height) > -1))
		{
			float offset_X;
			int center_Y = bl_temp->now_y + bl_temp->btn.height/2;

			//�������Ļ�ϰ벿�ֵĻ�
			if(center_Y < 300)
				offset_X = 800.0 - bl_temp->btn.width*0.6 - (center_Y/300.0)*bl_temp->btn.width*0.4;
			else
				offset_X = 800.0 - bl_temp->btn.width*0.6 - (1.0-((center_Y-300.0)/300.0))*bl_temp->btn.width*0.4;

			bl_temp->setnextpos(offset_X,nowY);
		}
		else
		{
			bl_temp->setnextpos((800.0-bl_temp->btn.width*0.6),nowY);
		}
	}
}

void clsBtn(int index)
{
	bl1_pos = bl1;

	while(bl1_pos)
	{
		if(bl1_pos->index == index)
		{
			bl1_pos->inCls = true;
			bl1_pos->setnextpos(850,bl1_pos->pos_y);
			break;
		}
		bl1_pos = bl1_pos->next;
	}
}

//btn_over�¼�
int btn_over()
{
	int x,y;
	GetMousePos(x,y);

	btnlist *pos = bl1;
	while(pos)
	{
		//ɸѡ����Ļ�л��Ƶ�btn����
		if(pos->showOn)
		{
			if((x>pos->now_x) && (x<(pos->now_x+pos->btn.width)))
			{
				if((y>pos->now_y) && (y<(pos->now_y+pos->btn.height)))
				{
					if(btnHeight>600)
						Top_Y = -(y/600.0)*(btnHeight-600);
					else
						Top_Y = (y/600.0)*(600-btnHeight);

					return (pos->index);
				}
			}
		}
		pos = pos->next;
	}

	return -1;
}

//btn_click�¼�
void btn_click(int index)
{
	click_index = -1;
	if(index!=-1)
	{
		//����������
		if(MouseIsDown(0))
		{
			down_index = index;
			//δ����:��ʾbtn_down
		}
		else
		{
			if(down_index != -1)
			{
				//����click�¼�
				down_index = -1;
				click_index = index;
				return;
			}
		}
	}
	else
		down_index = -1;
}

void btn_event()
{
	over_index = btn_over();
	btn_click(over_index);
}