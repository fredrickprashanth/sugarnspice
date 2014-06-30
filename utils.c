#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.h"
void 
time_it()
{
	static int flag_in = 0;
	static struct timespec ts[2];
	double delta_ts;
	clock_gettime(CLOCK_MONOTONIC, &ts[flag_in]);
	if(flag_in)
	{
		delta_ts = delta_timespec(ts[1], ts[0]);
		printf("delta_ts = %lf ms\n", delta_ts);
	}
	flag_in = !flag_in;
}

void
_sig_term_handler(int sno){
	printf("Bye..It was fun hogging..!");
	_exit(0);
}


