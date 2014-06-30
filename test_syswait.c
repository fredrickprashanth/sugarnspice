#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_CPUS 2

void *thread_code(void *args)
{
	int j;
	int pid2;
	for (j = 0; j < 1000; j++) {
		pid2 = fork();
		if (pid2 == 0)
		while(1) { sleep(1000); }
	}

	while (1) {
		int status;
		if (waitpid(-1, &status, WNOHANG)) {
			printf("! %d\n", errno);
		}

	}
	exit(0);

}

/*
* non-blocking waitpids in tight loop, with many children to go through,
* done on multiple thread, so that they can "pass the torch" to eachother
* and eliminate the window that a writer has to get in.
*
* This maximizes the holding of the tasklist_lock in read mode, starving
* any attempts to take the lock in the write mode.
*/
int main(int argc, char **argv)
{
	int i;
	pthread_attr_t attr;
	pthread_t threads[NUM_CPUS];
	for (i = 0; i < NUM_CPUS; i++) {
		assert(!pthread_attr_init(&attr));
		assert(!pthread_create(&threads[i], &attr, thread_code, NULL));
	}
	while(1) { sleep(1000);}
return 0;
} 
