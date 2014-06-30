#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

int main() {
	sem_t mysem;
	struct timespec ts;
	if (sem_init(&mysem, 0, 0) < 0) {
		perror("sem_init failed");
	}
	if ( clock_gettime(CLOCK_REALTIME, &ts) < 0) {
		perror("clock_gettime failed");
	}
	ts.tv_sec += 5;
	if (sem_timedwait(&mysem, &ts) < 0) {
		perror("sem_timedwait failed");
	}
	
}
