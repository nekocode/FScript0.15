#include "sleep.h"
extern float hz;

bool time::sleep(int time)       //  timeΪ1/1000��
{
	count++;
	if(count>(hz*time))
	{
		count = 0.0;
		return true;
	}
	return false;
}