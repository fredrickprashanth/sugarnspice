#include <stdio.h>
#include <time.h>
int
main(){
	char buf[64];
	time_t curr_time;
	struct tm curr_tm;
	time(&curr_time);
	localtime_r(&curr_time, &curr_tm);
	strftime(buf, sizeof(buf), "%F %T", &curr_tm);
	printf("%s\n", buf);
	return 0;
}
