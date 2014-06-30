#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
void
term_hdlr(int signo)
{
	printf("Quiting\n");
	exit(0);
}
int main()
{
	void *ptr;
	unsigned long order = 1;
	signal(SIGINT, term_hdlr);
	signal(SIGTERM, term_hdlr);
	while((ptr = malloc(order<<=1)))
	{
		memset(ptr, 'a', order);
		printf("Allocated %lu bytes\n", order);
	}
	order>>=1;
	for(;order;order<<=1)
	{
		while((ptr = malloc(order)))
		{
			memset(ptr, 'v', order);
			printf("Allocated %lu bytes\n", order);
		}
	}
	printf("Can't allocate more\n");
	pause();
	return 0;
}
