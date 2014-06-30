#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sched.h>
void
term_hdlr(int signo)
{
	printf("Quiting\n");
	exit(0);
}

#define MAX_PIDS  3
int pids[MAX_PIDS];

void
alrm_hdlr(int signo)
{
	int i;
	for(i=0; i<MAX_PIDS; i++)
	{
		kill(pids[i], SIGINT);
	}
}

int 
main()
{
	int childs = 0;
	int child_id = -1;
	int i;
	int status;
	time_t curr_time;
	char pname[16];

	signal(SIGTERM, term_hdlr);
	signal(SIGINT, term_hdlr);
	signal(SIGALRM, alrm_hdlr);
	
	int cpu = 0;
	cpu_set_t cpumask;
	CPU_ZERO(&cpumask);
	CPU_SET(cpu, &cpumask);
	if(sched_setaffinity(0, sizeof(cpumask), &cpumask)<0)
	{
		perror("sched_setaffinity");
		exit(-1);
	}

	alarm(40);
	for(i=0; i<MAX_PIDS; i++)
	{
		pids[childs] = 0;
		pids[childs++] = fork();
		if(pids[childs - 1] == 0)
		{
			child_id = childs;
			sprintf(pname, "child%d", child_id);
			prctl(PR_SET_NAME, pname, NULL, NULL, NULL);
			break;
		}
		printf("child%d pid=%d created\n", childs, pids[childs-1]);
			
	}
	if(child_id != -1)
	{
		printf("child_id value = %d\n", child_id);
		if(child_id > 1)
		{
			struct timeval tv;
			tv.tv_sec = 60;
			tv.tv_usec = 0;
			select(1, NULL, NULL, NULL, &tv);
			printf("child%d done sleeping\n", child_id);
		}
			
		while(1)
		{
			for(i=0;i<9999999;i++);
			for(i=0;i<9999999;i++);
			time(&curr_time);
			printf("%lu:%d\n",curr_time, childs);
		}
	}

	for(i=0; i<MAX_PIDS; i++)
	{
		waitpid(pids[i], &status, 0);
	}
	
	return 0;
	
}
