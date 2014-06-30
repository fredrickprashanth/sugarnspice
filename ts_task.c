#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
int
main()
{
	cpu_set_t cpumask;
	CPU_ZERO(&cpumask);
	CPU_SET(0, &cpumask);
	sched_setaffinity(0, sizeof(cpumask), &cpumask);
	
	while(1){
		//do something, i don know what
	}
	return 0;
}
	 
