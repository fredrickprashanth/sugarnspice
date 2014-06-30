#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define BUF_SZ (4096*1024)
char buf[BUF_SZ];

void
sigusr1_hdlr(int num, siginfo_t *info,
		void *unused) {

	printf("SIGUSR1 recived\n");	
}

int
verify_buf(char pat) {
	
	int i;
	for (i = 0; i<sizeof(buf); i++) {
		if (buf[i] != pat)
			return 0;
	}
	return 0;
}

void child_main() {
	
	struct timespec ts;

	ts.tv_sec = 0;
	ts.tv_nsec = 100000;
	while(1) {
		kill(getppid(), SIGUSR1);
		nanosleep(&ts, NULL);
	}
}

int
main(int argc, char *argv[]) {
	
	char pat = 0x1;
	struct timespec ts;
	int child_pid;
	struct sigaction act;

	ts.tv_sec = 0;
	ts.tv_nsec = 10000;

	memset(&act, 0, sizeof(act));
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = &sigusr1_hdlr;
	if (sigaction(SIGUSR1, &act, NULL) < 0) {
		perror("sigaction failed");
		return 0;
	}

	if (!(child_pid = fork())) {
		child_main();
		return 0;
	}
	
	while(1) {
		memset(buf, pat, sizeof(buf));
		nanosleep(&ts, NULL);
		if (!verify_buf(pat)) {
			printf("buf verify failed\n");
			break;
		}
		pat++;
	}
	if (kill(child_pid, SIGTERM) < 0) {
		perror("kill child failed");
		return 0;
	}
	if (waitpid(0, NULL, 0) < 0){
		perror("waitpid error");
	}

	return 0;
}
