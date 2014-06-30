#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include <sys/timerfd.h>
#include <sys/select.h>
#include <sys/signalfd.h>
#include <sys/types.h>

int
main(int argc, char *argv[])
{
	int timer_fd, signal_fd;
	int max_fd;
	fd_set read_fds;
	sigset_t sigmask;
	uint64_t timer_expiries; 
	struct itimerspec its;
	int timer_expiry_sec;
	int iterations, iter = 0;
	unsigned long *ts_delta;
	struct timespec ts1, ts2;

	if(argc<3)
	{
		printf("timerfd_test <secs> <iters>\n");
		return 1;
	}

	sscanf(argv[1], "%d", &timer_expiry_sec);
	sscanf(argv[2], "%d", &iterations);
	
	ts_delta = (unsigned long*)malloc(sizeof(unsigned long)*iterations);

	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGINT);
	sigaddset(&sigmask, SIGTERM);
	signal_fd = signalfd(-1, &sigmask, 0);
	if(signal_fd < 0)
	{
		perror("signalfd:");
		return 2;
	}
	sigprocmask(SIG_BLOCK, &sigmask, NULL);

	timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	if(timer_fd<0)
	{
		perror("timerfd_create:");
		return 3;
	}
	memset(&its, 0, sizeof(its));

	its.it_value.tv_sec = timer_expiry_sec;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = timer_expiry_sec;
	its.it_interval.tv_nsec = 0;
	max_fd = timer_fd > signal_fd? timer_fd:signal_fd;
	while(iter < iterations)
	{
		FD_ZERO(&read_fds);
		FD_SET(timer_fd, &read_fds);
		FD_SET(signal_fd, &read_fds);
		
		timerfd_settime(timer_fd, 0, &its, NULL);

		clock_gettime(CLOCK_MONOTONIC, &ts1);
		if(select(max_fd + 1, &read_fds, NULL, NULL, NULL)<0)
		{
			perror("select:");
			break;
		}
		if(FD_ISSET(signal_fd, &read_fds))
		{
			break;
		}
		if(FD_ISSET(timer_fd, &read_fds))
		{
			read(timer_fd, &timer_expiries, sizeof(timer_expiries));
			//printf("timer caught expiries=%llu\n", timer_expiries);

		}
		clock_gettime(CLOCK_MONOTONIC, &ts2);
		ts_delta[iter] = ts2.tv_sec*1000000000 + ts2.tv_nsec
				 - ts1.tv_sec*1000000000 - ts1.tv_nsec;
		printf("iter = %d ts_delta = %lu\n", iter, ts_delta[iter]);
		iter++;
		
	}
	printf("quiting\n");
	close(timer_fd);
	close(signal_fd);
	return 0;
}

