
#include <signal.h>
#include <stdio.h>
#include <time.h>

void on_exit_fn (int x, void *arg) {
	printf("on_exit function called\n");
}
void sigabrt_hdlr(int x) {
	printf("Got aborted\n");
}
void sigusr1_hdlr(int x) {
	printf("Got SIGUSR1\n");
}
main () {
	signal(SIGABRT, sigabrt_hdlr);
	signal(SIGUSR1, sigusr1_hdlr);
	while(1) {
		printf("%ld\n", time(NULL));
		pause();
	}
	//on_exit(on_exit_fn, NULL);
	pause();
}
