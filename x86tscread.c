#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <sys/types.h>
static __inline__ unsigned long long rdtsc(void)
{
  unsigned long long int x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
     return x;
}
int
main(int argc, char *argv[])
{
	int cpu = 0;
	int loop=99999;
	cpu_set_t cpumask;
	CPU_ZERO(&cpumask);
	if(argc==2)
		sscanf(argv[1], "%d", &cpu);
	CPU_SET(cpu, &cpumask);
	if(sched_setaffinity(0, sizeof(cpumask), &cpumask)<0)
	{
		perror("sched_setaffinity:");
	}
	while(loop--);
	long long tsc = rdtsc();
	printf("cpu%d:%llu\n", cpu, tsc);
	return 0;
}
