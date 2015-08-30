#include "variable.h"

var *newv;
var *nowv;
var *head;

void addVar(char *name,int val)
{
	var *newv = new var;
	strcpy_s(newv->varname,name);
	newv->val = val;

	if(!head)
	{
		head = newv;
		nowv = newv;
		nowv->next = 0;
	}
	else
	{
		nowv->next = newv;
		nowv = newv;
		nowv->next = 0;
	}
}

var* search(char *name)
{
	var* tempv;
	tempv = head;

	while(tempv!=0)
	{
		if(!strcmp(tempv->varname,name))
		{
			return tempv;
		}
		tempv = tempv->next;
	}
	return 0;
}

int getVar(char *name)
{
	if(search(name))
		return (search(name)->val);
	return 0;
}

void setVar(char *name,int val)
{
	if(search(name))
		search(name)->val = val;
}