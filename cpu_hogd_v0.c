#define _GNU_SOURCE
#include <sched.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

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
	char c;
	int daemon = 0;
	
	while((c = getopt(argc, argv, "d::c:s:"))!=-1)
	{
			
		switch(c)
		{
			case 'd': //daemon mode
				daemon = 1;
				break;
			case 'c': //cpu mask
				sscanf(optarg, "%d", &mask);
				break;
			case 's': //seconds
				sscanf(optarg, "%d", &secs);
				break;
			default:
				break;
		}
		if(daemon)
			break;
	}

	#define CPUHOG_PIPE "/tmp/cpuhogd.pipe"
	int cpuhog_pipe_fd;
	char buf[128], cmd[32], *buf_ptr;
	int bytes;
	if(daemon)
	{
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

		if(mknod(CPUHOG_PIPE, S_IFIFO|0666, 0)<0)
		{
			perror("mknod:");
			if(errno != EEXIST)
				exit(-1);
		}
		
		
		while(1)
		{
			cpuhog_pipe_fd = open(CPUHOG_PIPE, O_RDONLY);
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
				break;
			}
			buf[bytes] = 0;
			//printf("pid = %d bytes = %d buf = %s\n", getpid(), bytes, buf);
			buf_ptr = buf;
			sscanf(buf_ptr, "%s", cmd);
			while(*buf_ptr && buf_ptr < buf + bytes && *buf_ptr++!=' ');

			if(!strcmp(cmd, "quit"))
			{
				printf("cpuhogd exiting\n");
				exit(0);
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
					printf("cpuhogd exiting\n");
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
		/* remove pipe before quiting */
		unlink(CPUHOG_PIPE);
				
	}
	else /* !daemon */
	{
		cpuhog_pipe_fd = open(CPUHOG_PIPE, O_WRONLY);
		if(cpuhog_pipe_fd <0)
		{
			perror("open:");
			exit(-1);
		}
		sprintf(buf, "%d %d", mask, secs);
		bytes = write(cpuhog_pipe_fd, buf, strlen(buf));
		if(bytes < 0)
		{
			perror("write:");
		}
		close(cpuhog_pipe_fd);
	}

	return 0;
}
