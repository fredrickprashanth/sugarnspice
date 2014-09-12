#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/mount.h>
#include <signal.h>

#define __GNU_SOURCE
#define __USE_GNU
#include <sched.h>

#ifndef CLONE_NEWPID
#  define CLONE_NEWPID            0x20000000
#endif

int child(void *arg)
{
	pid_t pid;
	struct stat s;
	if (mount("proc", "/proc", "proc", 0, NULL)) {
		perror("mount");
		return -1;
	}
	pid = fork();
	if (pid < 0) {
		perror("fork");
		return -1;
	}
	if (!pid) {
		poll(0, 0 , -1);
		exit(-1);
	}
	poll(0, 0, -1);

	return 0;
}
pid_t clonens(int (*fn)(void *), void *arg, int flags)
{
	long stack_size = sysconf(_SC_PAGESIZE);
 	void *stack = alloca(stack_size) + stack_size;
	return clone(fn, stack, flags | SIGCHLD, arg);
}
int main(int argc, char *argv[])
{
	pid_t pid;
	struct stat s;
	char path[MAXPATHLEN];
	pid = clonens(child, NULL, CLONE_NEWNS);
	if (pid < 0) {
		perror("clone");
		return -1;
	}
	/* yes ugly.*/
	sleep(1);
	/* !! assumption : child of my child is pid + 1 
	 * any reliable simple solution is welcome :) */
	snprintf(path, sizeof(path), "/proc/%d/exe", pid + 1);
	if (lstat(path, &s)) {
		perror("lstat");
		exit(-1);
	}
	if (kill(pid, SIGKILL)) {
		perror("kill");
		return -1;
	}
	return 0;
}
