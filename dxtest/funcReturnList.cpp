#include "funcReturnList.h"
returnpos *newp;
returnpos *head;

void addReturn(long pos)
{
	returnpos *newp = new returnpos;
	newp->pos = pos;

	if(!head)
	{
		head = newp;
		head->next = 0;
	}
	else
	{
		newp->next = head;
		head = newp;
	}
}

long getReturn()
{
	if(head)
	{
		returnpos *tempP;
		long rult=head->pos;

		tempP = head;
		head = head->next;
		delete tempP;
		tempP = 0;

		return rult;
	}
	else
	{
		return -1;
	}
}

bool hasReturn()
{
	if(!head)
		return false;
	return true;
}

void deleteAllReturn()
{
	returnpos *tempP;
	tempP = head;

	while(head)
	{
		tempP = head;
		head = head->next;
		delete tempP;
		tempP = 0;
	}
}