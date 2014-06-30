#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>

void do_child(int);

void*
do_thread(void *arg) {

	int loops;
	int child_pid;
	int do_recursion = (int)arg;
	loops = 1;
	sleep(5);
	while(loops > 0) {
		child_pid = fork();
		if (child_pid == 0) {
			do_child(do_recursion);
			exit(0);
		}
		if (child_pid < 0) {
			perror("fork error");
			continue;
		}
		printf("%s: pid %d child_pid %d created\n",
			__FUNCTION__, getpid(), child_pid);
		usleep(10000);
		loops--;
		
	}

	pthread_exit(NULL);
}


void
create_threads(int nr_thrs, int do_recursion){

	int i;
	pthread_t *th;
	
	th = malloc(sizeof(*th)*nr_thrs);
	for(i = 0; i<nr_thrs; i++) {
		if(pthread_create(&th[i], NULL, do_thread, 
				(void*)do_recursion)) {
			perror("pthread_create error");
		}
		printf("%s: pid %d created thread %d\n",
			__FUNCTION__, getpid(), i);
	}
	for(i = 0; i<nr_thrs; i++) {
		pthread_join(th[i], NULL);
	}
}

void
do_child(int do_recursion){

	int nr_thrs;

	nr_thrs = 1;

	if (do_recursion) {
		printf("%s: pid %d creating thread\n",
			__FUNCTION__, getpid());
		create_threads(nr_thrs, !do_recursion);
	}
}


int
main() {
	
	int child_pid;
	
	child_pid = fork();
	if (child_pid == 0){
		do_child(1);
		exit(0);
	}
	printf("%s: pid %d child_pid %d created\n",
			__FUNCTION__, getpid(), child_pid);

	create_threads(1, 1);
	
	while(1) {
		child_pid = waitpid(0, NULL, 0);
		if (child_pid > 0) {
			printf("%s: pid %d child pid %d exited\n",
					__FUNCTION__, getpid(), child_pid);
		}
		else {
			perror("waitpid error");
			break;
		}
	}

	
	return 0;
}
