#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include <sched.h>

int main(int argc, char *argv[])
{
	unsigned long sleep_timeout, *ts_delta;
	int iters, i;
	struct timespec ts1, ts2, ts_timeout, ts_rem;
	struct sched_param param = { .sched_priority = 1 };
	
	if (argc < 3)
	{
		printf("test <sleep_timeout in ns> <iters>\n");
		return -2;
	}
	
	sscanf(argv[1], "%lu", &sleep_timeout);
	sscanf(argv[2], "%d", &iters);

	if(sched_setscheduler(0, SCHED_FIFO, &param) < 0)
	{
		perror("sched_setscheduler:");
		//exit(-1);
	}
	
	ts_delta = (unsigned long*)malloc(sizeof(unsigned long)*iters);
	i = 0;
	ts_timeout.tv_sec = 0;
	ts_timeout.tv_nsec = sleep_timeout;
	while (i++ < iters)
	{
		ts_rem.tv_sec = ts_rem.tv_nsec = 0;
		clock_gettime(CLOCK_MONOTONIC, &ts1);
#ifdef TEST_NANOSLEEP
		if (nanosleep(&ts_timeout, &ts_rem) < 0)
		{
			printf("ts_rem %lu %lu\n", ts_rem.tv_sec, ts_rem.tv_nsec);
			perror("nanosleep");
			break;
		}
#else
		usleep(sleep_timeout/1000);
#endif
		clock_gettime(CLOCK_MONOTONIC, &ts2);
		ts_delta[i] = ts2.tv_sec*1000000000 + ts2.tv_nsec
				 - ts1.tv_sec*1000000000 - ts1.tv_nsec;
		printf("iter = %d ts_delta = %lu\n", i, ts_delta[i]);
		
		
	}
	return 0;
}
		
