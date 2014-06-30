#define _GNU_SOURCE
#include <sched.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

volatile int do_looping;
void
sig_term_h(int sno){
	printf("Bye..It was fun hogging..!");
	exit(0);
}
void
sig_alrm_h(int sno){
	do_looping=0;
}
int 
main(int argc, char *argv[]){
	unsigned long loops;
	struct sched_param param = { .sched_priority = 40 };
	struct timespec *ts_req, *ts_rem;
	int cpu=0, secs=60;
	cpu_set_t cpumask;
	
	if(argc>1)
		sscanf(argv[1],"%d",&secs);
	printf("secs=%d\n",secs);

	if(secs<0 || secs>100)
		secs=60;

	CPU_ZERO(&cpumask);
	while(argc>2){
		sscanf(argv[argc-1],"%d",&cpu); //note: first cpu is 0
		CPU_SET(cpu, &cpumask);
		argc--;
	}

	if(sched_setscheduler(0, SCHED_FIFO, &param)<0)
	{
		perror("sched_setcheduler");
		exit(-1);
	}


	if(sched_setaffinity(0, sizeof(cpumask), &cpumask)<0)
	{
		perror("sched_setaffinity");
		exit(-1);
	}
	

	ts_req=(struct timespec *)malloc(sizeof(struct timespec));
	ts_rem=(struct timespec *)malloc(sizeof(struct timespec));
	
	signal(SIGINT, sig_term_h);
	signal(SIGALRM, sig_alrm_h);
	do_looping =1;
	while(1){
		do_looping = 1;
		alarm(secs);
		while(do_looping);
		ts_req->tv_sec=3;
		ts_req->tv_nsec=1000*1000*1;
		ts_rem->tv_sec=0;
		ts_rem->tv_nsec=0;
		while(nanosleep(ts_req, ts_rem) !=0 && ts_rem->tv_nsec !=0)
		{
			printf("nanosleep remaining %lus %lluns\n", ts_rem->tv_sec, ts_rem->tv_nsec);
			ts_req = ts_rem;
	
		}
	}
		
		
		
	return 0;
	
}
