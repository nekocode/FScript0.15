#include <string.h>
#define varNameLen 15

struct var
{
	int val;
	char varname[varNameLen];
	var *next;
};

void addVar(char *name,int val);
int getVar(char *name);
void setVar(char *name,int val);