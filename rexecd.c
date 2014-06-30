#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include<sys/stat.h>
#include<fcntl.h>

#define PORT 8989
#define BACKLOG 2
#define MSG_SZ 256
#define HOSTNAME_SZ 256

int main()
{
	int server_sd,client_sd,log_fd;
	struct sockaddr_in server_addr,client_addr;
	char msg[MSG_SZ+1],hostname[HOSTNAME_SZ],logfilename[100],cmd[MSG_SZ+20];
	socklen_t t;
	int temp=1;

	if((server_sd=socket(PF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket");
		exit(1);
	}

	if(setsockopt(server_sd,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int))<0)
	{
		perror("setsockopt");
		exit(1);
	}
	
	server_addr.sin_family==AF_INET;
	server_addr.sin_port=htons(PORT);
	server_addr.sin_addr.s_addr=INADDR_ANY;
	bzero(server_addr.sin_zero,sizeof server_addr.sin_zero);

	if(bind(server_sd,(struct sockaddr*)&server_addr,sizeof server_addr)<0)
	{
		perror("bind");
		exit(1);
	}
	if(listen(server_sd,BACKLOG)<0)
	{
		perror("listen");
		exit(1);
	}
	if(gethostname(hostname,HOSTNAME_SZ)<0)
	{
		perror("gethostname");
		exit(1);
	}
	strcpy(logfilename,"rexecd.log.");
	strcat(logfilename,hostname);
	if((log_fd=open(logfilename,O_WRONLY|O_APPEND|O_CREAT,S_IRWXU))<0)
	{
		perror("open");
		exit(1);
	}
	
	close(1); //close stdout
	dup(log_fd);
	close(log_fd);

	t=sizeof client_addr;
	daemon(1,1);
	while(1)
	{
	
		if((client_sd=accept(server_sd,(struct sockaddr*)&client_addr,&t))<0)
		{
			perror("accept");
			continue;
		}
		if(!fork())
		{
			close(server_sd);
			if(recv(client_sd,msg,MSG_SZ,0)<0)
			{
				perror("recv");
				close(client_sd);
				exit(1);
			}
			system("date");
			strcpy(cmd,"$");
			strcat(cmd,msg);
			strcat(cmd,"\n");
			write(1,cmd,strlen(cmd));
			//fflush(1);
			system(msg);
			close(client_sd);
			exit(0);
		}
		close(client_sd);
	}
	return 0;
}

