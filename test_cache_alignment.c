#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define CACHE_LINE_SZ (64)

struct my_data {
	unsigned int data;
#ifdef CACHE_ALIGNED
	char __pad[CACHE_LINE_SZ - sizeof(int)];
#endif
};

struct my_data *my_shared_data;


#define MAX_DATA (1<<31)
void*
my_thread(void *data) {

	int val;
	struct my_data *md = (struct my_data*)data;
	while(md->data < MAX_DATA) {
		val = md->data;	
		md->data++;
		assert(md->data == (val + 1));
	}
	return NULL;
}

#define NARGS 1

int
main(int argc, char *argv[]) {

	int nr_threads;
	pthread_t *threads;
	unsigned long long delta_ts;
	int i;
	
	if (argc < (NARGS+1)) {
		printf("test_cache_alignment <nr_threads>\n");
		return 0;
	}

	sscanf(argv[1], "%d", &nr_threads);
	printf("nr_threads = %d\n", nr_threads);

	threads = malloc(nr_threads*sizeof(pthread_t));
	my_shared_data = malloc(nr_threads*sizeof(struct my_data));
	
	struct timespec t1, t2;

	clock_gettime(CLOCK_MONOTONIC, &t1);
	
	for (i = 0; i < nr_threads; i++) {
		if (pthread_create(&threads[i], NULL, my_thread, &my_shared_data[i])) {
			perror("pthread_create");
		}
	}
	for (i = 0; i < nr_threads; i++)
		pthread_join(threads[i], NULL);

	clock_gettime(CLOCK_MONOTONIC, &t2);
	
	delta_ts = (t2.tv_sec - t1.tv_sec)*1000000 + (t2.tv_nsec - t1.tv_nsec)/1000;

	printf("delta ts = %d\n", delta_ts);

	return 0;

}
