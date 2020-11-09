#include "include/stdio.h"
#include "include/unistd.h"

int main()
{
	unsigned int ticks = get_boottime();  // Number of seconds elapsed since boot

	int hrs = ticks / 3600;
	int mins = (ticks % 3600) / 60;
	int secs = ( ticks % 3600) % 60;


    // We will prepend '0' to the hrs,mins, secs if they are less than 10
	char *zh = "", *zm = "", *zs = "";
	if(hrs < 10)
		zh = "0";
	if(mins < 10)
		zm = "0";
	if(secs < 10)
		zs = "0";

	printf("Time elapsed since boot: [%s%d:%s%d:%s%d]\n", zh, hrs, zm, mins, zs, secs);
}