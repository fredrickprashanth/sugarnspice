#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NARGS (2)

int update_int;
int max_update_iters;
int last_update_int;
pthread_mutex_t update_lock;

#ifdef WITH_LOCK
static void inline update_int_lock() {
	pthread_mutex_lock(&update_lock);
}
static void inline update_int_unlock() {
	pthread_mutex_unlock(&update_lock);
}
#else
static void inline update_int_lock() { }
static void inline update_int_unlock() { }
#endif



void*
update_int_thread(void *data) {
	
	int i;
	last_update_int = (int)data; 
	for (i = 0; i<max_update_iters; i++) {
		update_int_lock();
		update_int++;
		update_int_unlock();
	}
	pthread_exit(NULL);
}

int 
main(int argc, char *argv[]) {

	int nr_threads;
	pthread_t *thr;
	int i;
	
	if (argc < (NARGS+1)) {
		printf("test_int_update <nr_threads> <max_iters>\n");
		return 0;
	}

	sscanf(argv[1], "%d", &nr_threads);
	sscanf(argv[2], "%d", &max_update_iters);

	pthread_mutex_init(&update_lock, NULL);

	thr = (pthread_t *)malloc(sizeof(pthread_t)*nr_threads);
	
	for (i = 0; i<nr_threads; i++) {
		if (pthread_create(&thr[i], NULL, update_int_thread, (void*)i)) {
			printf("pthread_create failed\n");
			return -1;
		}
	}

	for (i = 0; i<nr_threads; i++) {
		pthread_join(thr[i], NULL);
	}
	
	printf("last_update_int = %d\n", last_update_int);
	if (update_int == (max_update_iters*nr_threads)) {
		printf("PASSED\n");
	} else {
		printf("FAILED update_int = %d nr_threads = %d max_iters = %d\n",
			update_int, nr_threads, max_update_iters);
	}

	return 0;
}

