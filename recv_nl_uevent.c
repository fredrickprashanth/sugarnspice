#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <signal.h>

#include <linux/netlink.h>

#include <sys/select.h>


void process_uevent_buf(char *buf, int buflen);

int main()
{
	
	struct sockaddr_nl nls;
	char buf[8192];
	int buflen;
	char *uevent_envp[20];
	int envi;
	
	int nl_fd, max_fd;
	fd_set fds;	
	int fdcount;
	
	int i;

	struct iovec iov;
	struct msghdr msg;

	sigset_t sigmask;
	int sigmask_fd;

	memset(&nls, 0 ,sizeof(nls));
	nls.nl_family = AF_NETLINK;
	nls.nl_pid = 0;
	nls.nl_groups = 1;

	nl_fd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);	
	if(nl_fd<=0)
	{
		perror("socket");
		return -1;
	}
	if(bind(nl_fd, (void *)&nls, sizeof(nls)))
	{	
		perror("bind");
		close(nl_fd);
		return -1;
	}
	
	

	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGINT);
	sigaddset(&sigmask, SIGTERM);
	sigaddset(&sigmask, SIGQUIT);
	sigmask_fd = signalfd(-1, &sigmask, 0);
	
	if(sigmask_fd <0)
	{
		perror("signalfd");
		return -1;
	}
	
	sigprocmask(SIG_BLOCK, &sigmask, NULL);


	while(1)
	{
		FD_ZERO(&fds);
		FD_SET(nl_fd, &fds);
		FD_SET(sigmask_fd, &fds);
		max_fd = sigmask_fd > nl_fd? sigmask_fd: nl_fd ;

		fdcount = select(max_fd + 1, &fds, NULL, NULL, NULL);
		if(fdcount < 0)
		{
			perror("select");
			close(nl_fd);
			return -1;
		}
		
		if(FD_ISSET(sigmask_fd, &fds))
		{
			printf("exiting\n");
			close(nl_fd);
			return 0;
		}
			 
		memset(buf, 0 , sizeof(buf));
		
		iov.iov_base = buf;
		iov.iov_len = sizeof(buf);
		
		memset(&msg, 0, sizeof(msg));
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		msg.msg_name = &nls;
		msg.msg_namelen = sizeof(nls);

		buflen =  recvmsg(nl_fd, &msg, 0);
		if(buflen < 0)
		{
			perror("recvmsg");
		}
		i=0;
		/*
		while(i<buflen)
		{
			i+=printf("%s\n",buf+i);
		}
		*/	
		/*
                envi=0;
                uevent_envp[envi++] = buf;
                while(i<buflen)
                {
                        if(!buf[i++])
                                uevent_envp[envi++] = buf + i;  
                }
		for(i=0;i<envi;i++)
			printf("%s ", uevent_envp[i]);
		printf("\n");
		*/
		//for(i=0; i<buflen; i++)
		//	printf("%c",buf[i]);
		process_uevent_buf(buf, buflen);
	}
	return 0;
}
		
	
void process_uevent_buf(char *buf, int buflen){


  #define UEVENT_ENV_ACTION_STR           "ACTION="
    #define UEVENT_ENV_SUBSYSTEM_STR        "SUBSYSTEM="
    #define UEVENT_ENV_DEVPATH_STR          "DEVPATH="
    #define UEVENT_ENV_DEVTYPE_STR          "DEVTYPE="
    #define UEVENT_ENV_PHYSDEVBUS_STR       "PHYSDEVBUS="
    #define UEVENT_ENV_PHYSDEVDRIVER_STR    "PHYSDEVDRIVER="
    #define UEVENT_ENV_PHYSDEVPATH_STR    "PHYSDEVPATH="
    #define UEVENT_ENV_SLOT_STR             "SLOT="

    enum uevent_env_indices {
        UEVENT_ENV_ACTION,
        UEVENT_ENV_SUBSYSTEM,
        UEVENT_ENV_DEVPATH,
        UEVENT_ENV_DEVTYPE,
        UEVENT_ENV_PHYSDEVBUS,
        UEVENT_ENV_PHYSDEVDRIVER,
        UEVENT_ENV_PHYSDEVPATH,
        UEVENT_ENV_SLOT,
        UEVENT_ENV_MAX
    };

    char *uevent_env_str[] = {
        UEVENT_ENV_ACTION_STR,
        UEVENT_ENV_SUBSYSTEM_STR,
        UEVENT_ENV_DEVPATH_STR,
        UEVENT_ENV_DEVTYPE_STR,
        UEVENT_ENV_PHYSDEVBUS_STR,
        UEVENT_ENV_PHYSDEVDRIVER_STR,
        UEVENT_ENV_PHYSDEVPATH_STR,
        UEVENT_ENV_SLOT_STR,
    };

	char *uevent_envp_vals[20];
	char *uevent_envp;

	int matches = 0;
	int j;	

	uevent_envp = buf;
	for(j=0; j<UEVENT_ENV_MAX; uevent_envp_vals[j] = NULL, j++);

	while(uevent_envp < buf + buflen)
	{
		for(j=0; j<UEVENT_ENV_MAX; j++)
		{
			if(!strncmp(uevent_envp, uevent_env_str[j], strlen(uevent_env_str[j])))
			{
				uevent_envp_vals[j] = uevent_envp + strlen(uevent_env_str[j]);
				matches++;
				//printf("envp = %s\n", uevent_envp);
			}
		}
		while(*uevent_envp++);
	}
	for(j=0; j<UEVENT_ENV_MAX; j++)
		printf("%s = %s\n", uevent_env_str[j], uevent_envp_vals[j]);

	/*
	    if (
        uevent_envp_vals[UEVENT_ENV_SUBSYSTEM] &&
        strcmp(uevent_envp_vals[UEVENT_ENV_SUBSYSTEM], "block") &&
        uevent_envp_vals[UEVENT_ENV_PHYSDEVBUS] &&
        !strcmp(uevent_envp_vals[UEVENT_ENV_PHYSDEVBUS], "scsi") &&
        uevent_envp_vals[UEVENT_ENV_PHYSDEVDRIVER] &&
        !strcmp(uevent_envp_vals[UEVENT_ENV_PHYSDEVDRIVER], "sd") &&
        uevent_envp_vals[UEVENT_ENV_PHYSDEVPATH] &&
        strstr(uevent_envp_vals[UEVENT_ENV_PHYSDEVPATH], "usb") &&
        uevent_envp_vals[UEVENT_ENV_DEVTYPE] &&
        !strcmp(uevent_envp_vals[UEVENT_ENV_DEVTYPE], "disk") &&
        uevent_envp_vals[UEVENT_ENV_DEVPATH]
    )
    {   
        char *p;
        printf(": debug = %s\n", uevent_envp_vals[UEVENT_ENV_DEVPATH]+1);
        p = strtok(uevent_envp_vals[UEVENT_ENV_DEVPATH]+1, "/");
        printf(": debug = %s\n", p);
        p = strtok(NULL, "/");
        printf(": debug = %s\n", p);
        if (
            (p = strtok(uevent_envp_vals[UEVENT_ENV_DEVPATH]+1, "/")) &&
            !strncmp(p, "block", 5) &&
            (p = strtok(NULL, "/")) &&
            !strncmp(p, "sd", 2)
        )
        {   
		printf("gotcha..\n");
	}
    }
	*/
}

		
