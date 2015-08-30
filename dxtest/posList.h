#include <string.h>

#define PosNameLen 25

void addpos(char* name,long pos,bool isfunc=false);
long getpos(char* name,bool isfunc=false);
void delallpos();


struct poslist
{
	char name[PosNameLen];
	long pos;
	poslist *next;
};