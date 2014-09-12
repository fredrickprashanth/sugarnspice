#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


int malloc_cnt;
int *child_done;
#define USE_MMAP 1
#define PAGE_SZ 4096
struct alloc_info {
	void *addr;
	int size;
} *ptrs;

static void*
xalloc(int size)
{
	void *ptr;
#if USE_MMAP
	ptr = mmap(NULL, PAGE_SZ, PROT_READ|PROT_WRITE,
				MAP_ANON|MAP_PRIVATE, -1, 0);
	if (ptr == MAP_FAILED)
		return NULL;
#else
	ptr = malloc(size);
#endif
	return ptr;
}
static void
xfree(void *ptr, int size) {
#if USE_MMAP
	munmap(ptr, size);
#else
	free(ptr);
#endif
}

	
static void 
do_mallocs() {
	int i, size;
	struct alloc_info *ptr;
	srandom(time(NULL));
	for(i=0;i<malloc_cnt;i++) {
		ptr = ptrs + i;
		if (ptr->addr)
			xfree(ptr->addr, ptr->size);
		size = random()%(PAGE_SZ+1);
		ptr->addr = xalloc(size);
		if (!ptr->addr) {
			perror("malloc failed");
			return;
		}
		ptr->size = size;
		//printf("mmap[%d] = %p\n", i, ptr->addr);
		memset(ptr->addr, 'z', ptr->size);
	}
	for(i=0;i<malloc_cnt;i++) {
		ptr = ptrs + i;
		if(random()&0x1) {
			xfree(ptr->addr, ptr->size);
			ptr->addr = NULL;
			ptr->size = 0;
		}
	}
}

#define MAX_MAP_DELTA 9000 
int
clone_fn(void *data) {
	struct timespec *ts1, *ts2, tt;
	unsigned long int t1, t2, t_delta, sleep_time;
	int i, nr_frees = 0;
	struct alloc_info *ptr;

	ts2 = &tt;
	ts1 = data;
	clock_gettime(CLOCK_MONOTONIC, ts2);
	t2 = ts2->tv_sec*1000000 + ts2->tv_nsec/1000;
	t1 = ts1->tv_sec*1000000 + ts1->tv_nsec/1000;
	t_delta = t2-t1;
	for (i=0; i<malloc_cnt; i++) {
		ptr = ptrs + i;
		if (ptr->addr) {
			memset(ptr->addr, 'x', ptr->size);
		}
		else {
			nr_frees++;
		}
	}
	printf("t_delta = %ld usec nr_frees %d\n", t_delta, nr_frees);
	sleep_time = (unsigned long int) &ptr;
	sleep_time >>=8;
	sleep_time &= 0xf;
	printf("sleep time %ld\n", sleep_time);
	sleep(sleep_time);
	*child_done  = 1;
	return 0;
}
	

#define NARGS 3
int
main(int argc, char *argv[]) {
	struct timespec *t1;
	void *child_stack;
	int clone_flags = 0, do_clone_vm;
	int iters;
	int child_status, child_pid;
	int i;


	if (argc < (NARGS+1))
	{
		printf("test_clone <malloc_cnt> <clone_vm> <iters>\n");
		return -2;
	}

	//clone_flags = CLONE_FS|CLONE_FILES;

	sscanf(argv[1], "%d", &malloc_cnt);
	ptrs = (struct alloc_info *)malloc(
			sizeof(struct alloc_info)*malloc_cnt);
	sscanf(argv[2], "%d", &do_clone_vm);
	if (do_clone_vm)
		clone_flags |= CLONE_VM;
	
	sscanf(argv[3], "%d", &iters);
	
	child_done = (int *)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, 
			MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	if (child_done == MAP_FAILED) {
		perror("map failed");
		return -2;
	}

	t1 = mmap(NULL, sizeof(struct timespec), PROT_READ|PROT_WRITE, 
			MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	if (t1 == MAP_FAILED) {
		perror("mmap failed");
		return -2;
	}
	//dummy mallocs
	malloc(PAGE_SZ);
	malloc(PAGE_SZ);

#define CHILD_STACK_SZ (4*4096)
	child_stack = malloc(CHILD_STACK_SZ);
	if(!child_stack) {
		perror("malloc");
		printf("child stack allocation failed\n");
		return -3;
	}
	for (i = 0;i<iters; i++) {
		do_mallocs();
		clock_gettime(CLOCK_MONOTONIC, t1);
		*child_done = 0;
		child_pid = clone(clone_fn, child_stack+CHILD_STACK_SZ-1, clone_flags, t1);
		if (child_pid < 0) {
			perror("clone failed");
			return -1;
		}
		printf("child %d created\n", child_pid);
	}
	i = 0;
	while(1) {
		child_pid = waitpid(-1, &child_status, WNOHANG);
		if (child_pid > 0) {
			printf("child %d exited\n", child_pid);
			i++;
			if (i == iters) break;
		}	
		sleep(1);
	}

	return 0;
}
