#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<unistd.h>
#include<netdb.h>
#include<string.h>

#define PORT 8989
#define MSG_SZ 256

int main(int argc,char **argv)
{
	int client_sd;
	struct sockaddr_in server_addr;
	struct hostent *he;
	char msg[MSG_SZ],rexecd_host[100];

	FILE *fp=NULL;


	if((fp=fopen("rexecd.hosts","r"))==NULL)
	{
		perror("fopen");
		exit(1);
	}

		server_addr.sin_family=AF_INET;
		server_addr.sin_port=htons(PORT);
		bzero(server_addr.sin_zero,sizeof server_addr.sin_zero);

	while(!feof(fp))
	{
		fscanf(fp,"%s",rexecd_host);
		if(!fork())
		{
			if((client_sd=socket(PF_INET,SOCK_STREAM,0))<0)
			{
				perror("socket");
				continue;
			}

			if((he=gethostbyname(rexecd_host))==NULL)
			{
				herror("gethostbyname");
				continue;
			}

			server_addr.sin_addr=*((struct in_addr*)he->h_addr);

			if(connect(client_sd,(struct sockaddr*)&server_addr,sizeof server_addr)<0)
			{
				perror("connect");
				exit(1);
			}

			strcpy(msg,argv[1]);
			if(send(client_sd,msg,MSG_SZ,0)<0)
			{
				perror("send");
			}
			close(client_sd);
			exit(0);
		}
	}
	return 0;
}





