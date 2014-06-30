#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int pid, exit_pid;
	int status;
	char *dummy_environ[] = { NULL };
	if (argc < 2)
	{
		printf("please some command args\n");
		return -1;
	}

	if (!(pid = fork()))
	{
		execve(argv[1], &argv[1], dummy_environ);
		perror("execve");
	}
	else
	{
		printf("forked pid = %d\n", pid);
		while (1)
		{
			if ( (exit_pid = waitpid(0, &status, WNOHANG)) > 0)
				break;
			printf("waiting for any pid to quit\n");
			sleep(1);
		}
		
		printf("pid %d exited with value = %d\n", exit_pid, WEXITSTATUS(status));
		return 0;
	}
	return 0;
}
			
		
	
	
