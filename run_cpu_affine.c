#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/sysinfo.h>
#include<sys/mman.h>
#include<unistd.h>
#define __USE_GNU
#include<sched.h>
#include<ctype.h>

int main(int argc,char **argv)
{

	cpu_set_t mask;
	int cpu_id=argv[1][0]-'0';
	CPU_ZERO(&mask);
	CPU_SET(cpu_id,&mask);
	printf("binding to cpu_id %d\n",cpu_id);
	if(sched_setaffinity(0,sizeof(mask),&mask)==-1)
	{
		perror("could not set cpu affinity");
	}
	execvp(argv[2],&argv[2]);

}

