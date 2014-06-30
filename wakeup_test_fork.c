#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/futex.h>

volatile int main_can_run;

#define _GNU_SOURCE
#include <sys/syscall.h>
#define futex(uaddr, op, val, timeout, uaddr2, val3)  \
		syscall(SYS_futex , uaddr, op, val, NULL, NULL, NULL)
void
sig_alrm_h(int sno)
{
	main_can_run = 0;	
}
void sig_term_h(int sno)
{
	exit(0);
}
void *my_futex;
int the_futex;
int
main(int argc, char *argv[])
{
	int no_threads, my_sleep_time;
	int i, pid, my_id;
	int *my_children;
	
	
	if(argc < 3)
	{
		printf("wakeup_test <no_threads> <time>\n");	
		return 2;
	}

  	if((shm_fd = shm_open("my_futex"	, (O_CREAT | O_EXCL | O_RDWR),
                       (S_IREAD | S_IWRITE))) < 0) {
  	}
  else if((shm_fd = shm_open(STATE_FILE, (O_CREAT | O_RDWR),
                        (S_IREAD | S_IWRITE))) < 0) {
   /* Try to open the shm instance normally and share it with
    * existing clients
    */
    printf("Could not create shm object. %s\n", strerror(errno));
    return errno;
  } 


	no_threads = strtoul(argv[1], NULL, 10);
	my_sleep_time = strtoul(argv[2], NULL, 10);

	my_children = (int *)malloc(sizeof(int)*no_threads);

	the_futex = 1;
	my_futex = &the_futex;
	for(i = 0; i < no_threads; i++)
	{
		my_id = i + 1;
		pid = fork();
		if(!pid)
			break;
		my_children[i] = pid;
	}
	
	if(!pid)
	{
	
		signal(SIGTERM, sig_term_h);
		futex(my_futex, FUTEX_WAIT, 1, NULL, NULL, 0);
		printf("Woke up %d\n", my_id);
		while(1)
		{
			printf("%d\n", my_id);
		}
	}

	my_id = 20;
	printf("Going to wake up all the threads\n");
	sleep(1);
	signal(SIGALRM, sig_alrm_h);
	alarm(my_sleep_time);
	the_futex = 2;
	struct timespec ts1, ts2;
	clock_gettime(CLOCK_MONOTONIC, &ts1);
	futex(my_futex, FUTEX_WAKE, no_threads, NULL, NULL, 0);
	clock_gettime(CLOCK_MONOTONIC, &ts2);
	main_can_run = 1;
	while(main_can_run)
	{
		printf("%d\n", my_id);
	}
	printf("Marking END\n");
	for(i = 0; i < no_threads; i++)
	{
		kill(my_children[i], SIGTERM);
		waitpid(my_children[i], NULL, 0);
	}

	printf("Time for broadcast = %lu\n", ((ts2.tv_sec*1000000000 + ts2.tv_nsec) - (ts1.tv_sec*1000000000 + ts2.tv_nsec))/1000000);
	return 0;
	
}
