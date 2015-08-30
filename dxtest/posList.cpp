#include "posList.h"

poslist* newP_point;
poslist* head_point;
poslist* nowP_point;

poslist* newP_func;
poslist* head_func;
poslist* nowP_func;

void addpos(char* name,long pos,bool isfunc)
{
	if(!isfunc)
	{
		poslist *newP_point = new poslist;
		strcpy_s(newP_point->name,name);
		newP_point->pos = pos;
		if(head_point==0)
		{
			head_point = newP_point;
			nowP_point = newP_point;
			nowP_point->next = 0;
		}
		else
		{
			nowP_point->next = newP_point;
			nowP_point = newP_point;
			nowP_point->next = 0;
		}
	}
	else
	{
		poslist *newP_func = new poslist;
		strcpy_s(newP_func->name,name);
		newP_func->pos = pos;
		if(head_func==0)
		{
			head_func = newP_func;
			nowP_func = newP_func;
			nowP_func->next = 0;
		}
		else
		{
			nowP_func->next = newP_func;
			nowP_func = newP_func;
			nowP_func->next = 0;
		}
	}
}

long getpos(char* name,bool isfunc)
{
	poslist* tempP;

	if(!isfunc)
		tempP = head_point;
	else
		tempP = head_func;

	while(tempP!=0)
	{
		if(!strcmp(tempP->name,name))
		{
			return tempP->pos;
		}
		tempP = tempP->next;
	}

	return -1;
}

void delallpos()
{
	poslist *tempP,*tempP2;

	tempP = head_point;
	while(tempP)
	{
		tempP2 = tempP;
		tempP = tempP->next;
		delete tempP2;
		tempP2 = 0;
	}

	tempP = head_func;
	while(tempP)
	{
		tempP2 = tempP;
		tempP = tempP->next;
		delete tempP2;
		tempP2 = 0;
	}
}