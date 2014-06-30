#define _GNU_SOURCE
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/wait.h>

#include <sched.h>

void
sig_term_h(int sno){
	printf("Bye..It was fun hogging..!");
	exit(0);
}

void
sig_alrm_h_cpuhogger(int sno){
	printf("cpuhogger exiting\n");
	exit(0);
	
}
void sig_chld_defunct_reap(int sno){
	while(waitpid(-1, NULL, WNOHANG)>0);
}

volatile int cpuhogd_stop_hog;
void sig_alrm_h_cpuhogd(int sno){
	cpuhogd_stop_hog = 1;
}

int 
main(int argc, char *argv[]){

	int cpu=0, secs=60;
	int mask = 1;

	int cpuhog_pipe_fd;
	char buf[128], cmd[32], *buf_ptr;
	char cpuhogd_pipe_path[64];
	int bytes;

	cpu_set_t cpumask;
	struct sched_param param = { .sched_priority = 1 };

	if(fork()) return 0;
	umask(0);
	setsid();

	if(sched_setscheduler(0, SCHED_FIFO, &param) < 0)
	{
		perror("sched_setscheduler:");
		//exit(-1);
	}

	signal(SIGTERM, sig_term_h);
	signal(SIGCHLD, sig_chld_defunct_reap);
	signal(SIGALRM, sig_alrm_h_cpuhogd);

	sprintf(cpuhogd_pipe_path, "/tmp/cpuhogd.%d.pipe", getpid());

	if(mknod(cpuhogd_pipe_path, S_IFIFO|0666, 0)<0)
	{
		perror("mknod:");
		if(errno != EEXIST)
			exit(-1);
	}
	
	
	while(1)
	{
		cpuhog_pipe_fd = open(cpuhogd_pipe_path, O_RDONLY);
		if(cpuhog_pipe_fd < 0)
		{
			perror("open:");
			exit(-1);
		}
		bytes = read(cpuhog_pipe_fd, buf, sizeof(buf));
		if(bytes<0)
		{
			perror("read");
			close(cpuhog_pipe_fd);
			goto cpuhogd_quit;
		}
		buf[bytes] = 0;
		//printf("pid = %d bytes = %d buf = %s\n", getpid(), bytes, buf);
		buf_ptr = buf;
		sscanf(buf_ptr, "%s", cmd);
		while(*buf_ptr && buf_ptr < buf + bytes && *buf_ptr++!=' ');

		if(!strcmp(cmd, "quit"))
		{
			printf("cpuhogd exiting\n");
			goto cpuhogd_quit;
		}
		if(!strcmp(cmd, "jam"))
		{
			sscanf(buf_ptr, "%d", &secs);
			cpuhogd_stop_hog = 0;
			if(secs >0 && secs<120)
			{
				alarm(secs);
				while(!cpuhogd_stop_hog);
			}
		}/*cmd = jam */
		else
		if(!strcmp(cmd,"hydra"))
		{
			sscanf(buf_ptr, "%d %d", &mask, &secs);
		
			if(mask == -1 || secs == -1)
			{
				goto cpuhogd_quit;
				exit(0);
			}
			if(secs > 100) 
				continue;
			cpu = 0;
			//printf("pid = %d sec = %d mask = %d\n", getpid(), secs, mask);
			while(mask<16 && mask && cpu<4)
			{
				int last_bit;
				last_bit = mask & 1;
				mask>>=1;
				cpu++;
				if(!last_bit) continue;

				CPU_ZERO(&cpumask);
				CPU_SET(cpu-1, &cpumask);
				if(sched_setaffinity(0, sizeof(cpumask), &cpumask)<0)
				{
					perror("sched_setaffinity");
					//exit(-1);
				}
				if(!fork())
				{
					setsid();
					printf("cpuhogger/%d starting\n", cpu-1); 
					prctl(PR_SET_NAME, "cpuhogger", NULL, NULL, NULL);
					signal(SIGALRM, sig_alrm_h_cpuhogger);
					alarm(secs);
					while(1);
					exit(0);
				}
			}
		}/*cmd = hydra */
		
		/* close the pipe before going for next iteration */
		close(cpuhog_pipe_fd);
	}/* while */

	cpuhogd_quit:
	printf("cpuhogd exiting\n");
	/* remove pipe before quiting */
	unlink(cpuhogd_pipe_path);

	return 0;
}
