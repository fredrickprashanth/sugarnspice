#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <pthread.h>
#include <linux/futex.h>
#include <errno.h>


#define _GNU_SOURCE
#include <sys/syscall.h>
#define gettid()  \
		syscall(SYS_gettid ,NULL, NULL, NULL, NULL, NULL)
#define futex(uaddr, op, val, timeout, uaddr2, val3)  \
		syscall(SYS_futex , uaddr, op, val, NULL, NULL, NULL)
#define PAGE_SZ 4096
#define MAX_FILE_SZ PAGE_SZ*40

#define USE_CLONE 1
#ifdef SHOW_FILE
void
show_file(char *str)
{
	int fd = open(str, O_RDONLY);
	char buf[128];
	printf("===== PID %ld ========\n", gettid());
	while(read(fd,buf,sizeof(buf))>0)
		printf("%s", buf);
	close(fd);
	printf("\n=================\n");
}
#else
void show_file(char *str) { }
#endif
int nr_threads;
int nr_mmap_per_thread;
int mmap_sz;
int mmap_sleep_sec;
pthread_mutex_t print_mutex;
#if USE_CLONE
int *thread_done, *thread_start;
#endif 

#if USE_CLONE
int
#else
void *
#endif
do_inf_mmap(void *data)
{
	//int fd = (int)data;
	int i, ret ; 
	int *ptr;
	struct timespec ts_wait;
	ts_wait.tv_sec = 1;
	ts_wait.tv_nsec = 0;
#define MAX_MMAP_PER_THREAD (1<<10)
	printf("entering thread pid %ld\n", gettid());
	for (i=0; i<nr_mmap_per_thread; i++) {
		ptr = mmap(NULL, mmap_sz, PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANON, -1, 
			MAX_FILE_SZ + PAGE_SZ*(random()%10));
		*ptr = 0xac;
		memset(ptr, 0xab, mmap_sz);
		if (ptr == MAP_FAILED) {
			perror("mmap");
			exit(-2);
		}
		//pthread_mutex_lock(&print_mutex);
		//printf("pid:%ld mmap ptr %p\n", gettid(), ptr);
		//show_file("/proc/self/maps");
		//pthread_mutex_unlock(&print_mutex);
		//sleep(mmap_sleep_sec);
	}
	printf("done mmap thread pid %ld\n", gettid());
#if USE_CLONE
	while (futex(thread_start, FUTEX_WAIT, 1, NULL, NULL, 0)<0);
#endif
#if USE_CLONE
	printf("pid %ld: futex_wait thread_done = %d\n", gettid(), *thread_done);
	ret = futex(thread_done, FUTEX_WAIT, *thread_done, NULL, NULL, 0);
	printf("pid %ld: futex_wait_done thread_done = %d\n", gettid(), *thread_done);
	*thread_done = *thread_done + 1;
	while(futex(thread_done, FUTEX_WAKE, 1, NULL, NULL, 0)==0);
	printf("pid %ld: futex_wake thread_done = %d\n", gettid(), *thread_done);
	
#endif
	return 0;
}

#define NARGS 4
int
main(int argc, char *argv[])
{
	//int fd;
	int i, nr_woke = 0;
	//char fname[128];

#if USE_CLONE
	void **child_stack;
	int *child_pids;
#else
	pthread_t *child_pids;
#endif

	if (argc < (NARGS+1)) {
		printf("test_mmap2.o <nr_threads> "
			"<nr_mmap_per_thread> <mmap_sz><mmap_sleep_sec> \n");
		return -1;
	}
	
	sscanf(argv[1], "%d", &nr_threads);
	sscanf(argv[2], "%d", &nr_mmap_per_thread);
	sscanf(argv[3], "%d", &mmap_sz);
	sscanf(argv[4], "%d", &mmap_sleep_sec);

#if USE_CLONE	
	thread_done = (int *)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE,
				MAP_ANON|MAP_SHARED, -1, 0);
	thread_start = (int *)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE,
				MAP_ANON|MAP_SHARED, -1, 0);

	*thread_done = 0;	
	*thread_start= 0;	

	if (!(child_stack = 
		malloc(sizeof(void *)*nr_threads))) {
		perror("malloc");
		return -2;
	}
	if (!(child_pids = 
		(int *)malloc(sizeof(int)*nr_threads))) {
		perror("malloc");
		free(child_stack);
		return -2;
	}
#else
	if (!(child_pids = 
		(pthread_t *)malloc(sizeof(pthread_t)*nr_threads))) {
		perror("malloc");
		return -2;
	}
	
#endif
#if 0
	sprintf(fname, "%s.%d", "/tmp/mmap_test", getpid());
	fd = open(fname, O_CREAT| O_RDONLY, 0660);
	if(fd < 0)
	{
		perror("open");
		return 1;
	}
#endif 
#if !USE_CLONE
	pthread_mutex_init(&print_mutex, NULL);
#else
	*thread_done = 0;
	*thread_start = 0;
#endif
	for (i =0; i<nr_threads; i++) {

#if USE_CLONE
#define CHILD_STACK_SIZE (8*PAGE_SZ)
		child_stack[i] = malloc(CHILD_STACK_SIZE);
		if (!child_stack[i]) {
			perror("mmap");
			return -2;
		}
		child_pids[i] = clone(do_inf_mmap, 
			child_stack[i] + CHILD_STACK_SIZE-1, 
			CLONE_FILES|CLONE_VM,
			(void *)0);
		if (child_pids[i] < 0) {
			perror("clone");
			break;
		}
		printf("PID %d created\n", child_pids[i]);
#else
		if (pthread_create(&child_pids[i], NULL, 
				do_inf_mmap, (void *)0)) {
			perror("pthread_create");
			break;
		}
#endif
	}
#if USE_CLONE
	*thread_done = *thread_done +1;
	*thread_start = 1;
	while ((nr_woke += (futex(thread_start, FUTEX_WAKE, 1, NULL, NULL, 0))) < nr_threads) {
		printf("main:%ld: nr_woke = %d\n", gettid(), nr_woke);
		usleep(100);
	}
	usleep(1000);
	while (futex(thread_done, FUTEX_WAKE, 1, NULL, NULL, 0) == 0);
	while (*thread_done < nr_threads){
		printf("thread_done = %d\n", *thread_done);
		sleep(mmap_sleep_sec);
	}
	//wake the last thread.
	futex(thread_done, FUTEX_WAKE, 1, NULL, NULL, 0);
	futex(thread_done, FUTEX_WAIT, *thread_done, NULL, NULL, 0);

#else
	for (i =0; i<nr_threads; i++) {
		if (child_pids[i] > 0)
			pthread_join(child_pids[i], NULL);
	}
#endif
	printf("pid:%ld end of main thread\n",gettid());

	//show_file("/proc/self/maps");
	printf("\n");
	//pause();
	return 0;
}

