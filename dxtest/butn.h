#include "dxImage.h"
#include "dxinput.h"
//#include "textdraw.h"

#define btnTime 25
#define btnTextLen 195
#define PosNameLen 25

class btnlist
{
public:
	int index;
	wchar_t text[btnTextLen];
	char fucn[PosNameLen];

	bool showOn;
	bool inCls;
	//目标x,y
	int pos_x,pos_y;
	//当前x,y
	float now_x,now_y;

	dximg btn;
	//双向链表
	btnlist *front;
	btnlist *next;

	void drawbtn();
	void setnowpos(float x,float y);
	void setnextpos(float x,float y);
private:
	float a_x,a_y;
	float v0_x,v0_y;
//	float vt_x,vt_y;
};


void addBtn(int index,wchar_t* name,wchar_t* path);
void clsBtn(int index);
void btn_event();