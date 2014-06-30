#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "mycommon.h"
int nr_sum;
int nr_thrs;
int nr_forks;
int nr_mmaps;

void 
sigchld_handler(int signum) {
	int child_pid;
	while((child_pid = waitpid(-1, NULL, WNOHANG)) > 0){
		printf("pid %d child pid %d exited\n", 
			getpid(), child_pid);
		nr_sum--;
	}
}

#define TMP_DIR "/tmp"
void 
do_file_mmaps() {
	char **ptrs;
	int i, mmap_sz, fd, rnd_i;
	int num_mmaps;
	char fname[256];

	srand(time(NULL));

	num_mmaps = nr_mmaps;
	if (!nr_mmaps)
		num_mmaps = rand()&0xffff;

	ptrs = malloc(sizeof(ptrs)*num_mmaps);

	for (i=0; i<num_mmaps; i++) {
		mmap_sz = (rand()&0xf)<<12;
		if (!mmap_sz) mmap_sz = 4<<12;
		printf("random = %d\n", rand());
		srand(time(NULL));
		rnd_i = (rand())&0xffff;
		sprintf(fname, "%s/filemap.%d",
			TMP_DIR, i, rnd_i);
		fd = open(fname, O_RDWR, 0777);
		if (fd < 0) {
			fd = open(fname, O_CREAT|O_RDWR, 0777);
		}
		if (fd < 0) {
			perror("open failed");
			continue;
		}
		ftruncate(fd, mmap_sz);
		lseek(fd, 0, SEEK_SET);
		ptrs[i] = mmap(NULL, mmap_sz,
				PROT_READ|PROT_WRITE, 
				MAP_SHARED, fd, 0);
		if (ptrs[i] == MAP_FAILED) {
			perror("mmap failed");
			continue;
		}
		memset(ptrs[i], -1, mmap_sz);
		 
	}
	
}

void
do_anon_mmaps() {
	char **ptrs;
	int i, mmap_sz;
	int num_mmaps;

	if (!nr_mmaps)
		num_mmaps = rand()&0xffff;
	ptrs = malloc(sizeof(ptrs)*num_mmaps);
	for (i=0; i<num_mmaps; i++) {
		mmap_sz = (rand()&0xf)<<12;
		ptrs[i] = mmap(NULL, mmap_sz,
				PROT_READ|PROT_WRITE, 
				MAP_ANON, -1, 0);
		if (ptrs[i] == MAP_FAILED) {
			perror("mmap failed");
			continue;
		}
		memset(ptrs[i], -1, mmap_sz);
		 
	}
}
void 
do_child_task(void){
	
	int sleep_time;
	printf("child pid %d starts\n", getpid());
	while (1) {
		do_anon_mmaps();
		do_file_mmaps();
		sleep_time = rand()&0xffff;
		if (sleep_time < 10) 
			break;
		usleep(sleep_time);	
	}
	printf("child pid %d exiting\n", getpid());
	exit(0);
}

void*
do_waitpid_thread(void *arg){
	int child_pid;
	int do_pause = (int)arg;
	do {
		printf("tid %ld pid %d waitpid\n", gettid(), getpid());
		child_pid = waitpid(0, NULL, WNOHANG);
		if (child_pid > 0){
			printf("tid %ld pid %d child pid %d exited\n",
				gettid(), getpid(), child_pid);
			nr_sum--;
		}
		if (do_pause) sleep(1);
		if (rand() & 0x1)
				break;
	}
	while(1);
	pthread_exit(NULL);
}

void*
do_create_child_thread(void *arg) {

	int child_pid;
	int do_pause = (int)arg;
	int i, nr_childs;

	srand(time(NULL));
	nr_childs = nr_forks;
	while (1) { 
		if (!nr_forks)
			nr_childs = rand()&0xff;
		else
			nr_childs = nr_forks;

		for (i = 0; i<nr_childs; i++) {
			child_pid = fork();
			if (child_pid == 0){
				do_child_task();
				exit(0);
			}
			if (child_pid < 0) {
				perror("fork error");
				pthread_exit(NULL);
			}
			nr_sum++;
			printf("tid %ld pid %d child pid %d created\n",
				gettid(), getpid(), child_pid);
		}
		for (i = 0; i<nr_childs; i++) {
			child_pid = waitpid(0, NULL, 0);
			if (child_pid < 0) {
				perror("waitpid error");
			}
			else {
				printf("pid %d child_pid %d exited\n",
						getpid(), child_pid);
				nr_sum--;
			}
		}

		//pausing;
		if(do_pause) {
			printf("pausing pid %d tid %ld\n", getpid(), gettid());
			sleep(1);
		}
		else {
			break;
		}

	}
	
	printf("pid %d tid %ld exiting\n", getpid(), gettid());

	pthread_exit(NULL);
}

#define NARGS 3
int main(int argc, char *argv[]){

	int i;
	int arg_i;
	pthread_t thc, thw;
	pthread_attr_t attr;

	srand(time(NULL));

	if (argc < (NARGS+1)) {
		printf("test_tasklock <nr_thrs> <nr_forks> <nr_mmaps>\n");
		return -1;
	}
	
	arg_i = 1;
	sscanf(argv[arg_i++], "%d", &nr_thrs);
	sscanf(argv[arg_i++], "%d", &nr_forks);
	sscanf(argv[arg_i++], "%d", &nr_mmaps);
	printf("nr_thrs = %d\n", nr_thrs);
	printf("nr_forks = %d\n", nr_forks);
	printf("nr_mmaps = %d\n", nr_mmaps);
	

	signal(SIGCHLD, sigchld_handler);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	if (!nr_thrs)
		nr_thrs = rand()&0xfff;
	
	for (i = 0; i < nr_thrs; i++) {
		if (pthread_create(&thc, &attr, do_create_child_thread, (void *)1)) {
			perror("pthread_create failed");
			fprintf(stderr, "pthread create child_thread failed at i=%d\n", i);
			sleep(1);
		}
		if (pthread_create(&thw, &attr, do_waitpid_thread, (void*)1)){
			perror("pthread_create failed");
			fprintf(stderr, "pthread create waitpid thread failed at i=%d\n", i);
			sleep(1);
		}
		printf("thrs i = %d\n", i);
	}
	while(1) {
		printf("in while loop main\n");
		if (pthread_create(&thc, &attr, do_create_child_thread, (void *)0)) {
			perror("pthread_create failed");
			fprintf(stderr, "pthread create child_thread in while\n");
			sleep(1);
		}
		if (pthread_create(&thw, &attr, do_waitpid_thread, (void *)0)){
			perror("pthread_create failed");
			fprintf(stderr, "pthread create waitpid_thread in while\n");
			sleep(1);
		}
		/*
		pthread_join(thc, NULL);
		pthread_join(thw, NULL);
		*/
		sleep(1);
		printf("total sum = %d\n", nr_sum);
	}
	return 0;
}

