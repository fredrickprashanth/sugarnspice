#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

pthread_cond_t my_cond;
pthread_mutex_t my_mutex;
void*
my_thread_routine(void *arg)
{
	int my_id = (int)arg;
	pthread_mutex_lock(&my_mutex);
	pthread_cond_wait(&my_cond, &my_mutex);
	pthread_mutex_unlock(&my_mutex);
	while(1)
	{
		pthread_testcancel();
		printf("%d\n", my_id);
	}
	pthread_exit(NULL);
	
}
volatile int main_can_run;

void
sig_alrm_h(int sno)
{
	main_can_run = 0;	
}
int
main(int argc, char *argv[])
{
	int no_threads, my_sleep_time;
	int i;
	pthread_t *my_threads;
	int init_main_loop = 999999;	

	if(argc < 3)
	{
		printf("wakeup_test <no_threads> <time>\n");	
		return 2;
	}


	no_threads = strtoul(argv[1], NULL, 10);
	my_sleep_time = strtoul(argv[2], NULL, 10);

	my_threads = malloc(sizeof(pthread_t)*no_threads);
	for(i = 0; i <no_threads; i++)
	{
		if(pthread_create(my_threads + i, NULL, my_thread_routine, (void *)i+1))
		{
			perror("pthread_create:");
		}
	}
	
	signal(SIGALRM, sig_alrm_h);

	//let the main thread run at higher priority
	nice(-5);
	printf("Going to wake up all the threads\n");
	sleep(1);
	while(init_main_loop--);
	alarm(my_sleep_time);

	//Lets create the thunder
	struct timespec ts1, ts2;
	clock_gettime(CLOCK_MONOTONIC, &ts1);
	pthread_cond_broadcast(&my_cond);
	clock_gettime(CLOCK_MONOTONIC, &ts2);
	
	
	main_can_run = 1;
	while(main_can_run)
	{
		printf("0\n");
	}
	printf("Marking END\n");
	
	for(i = 0; i < no_threads; i++)
	{
		pthread_cancel(my_threads[i]);
	}
	
	for(i = 0; i < no_threads; i++)
	{
		pthread_join(my_threads[i], NULL);
	}

	printf("Time for broadcast = %lu\n", ((ts2.tv_sec*1000000000 + ts2.tv_nsec) - (ts1.tv_sec*1000000000 + ts2.tv_nsec))/1000000);
	return 0;
	
}
