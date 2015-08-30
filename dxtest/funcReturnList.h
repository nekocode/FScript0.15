void addReturn(long pos);
long getReturn();
void deleteAllReturn();
bool hasReturn();

struct returnpos
{
	long pos;
	returnpos *next;
};