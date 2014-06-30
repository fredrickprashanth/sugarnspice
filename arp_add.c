#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int sd;
	struct arpreq arpreq;
	struct sockaddr_in *sin;
	struct in_addr ina;

	if(argc<3)
	{
		printf("Usage: arp_add.o <IP> <MAC> <DEV>\n");
		return 0;
	}

	char *ip = argv[1];
	char *eaddr = argv[2];
	char *eth_dev;
	if(argc>3)
		eth_dev = argv[3];
	else
		eth_dev = "";
		
	int rc, i;
	struct ether_addr *eth_addr;

	memset(&arpreq, 0, sizeof(arpreq));
	sd = socket(AF_INET,SOCK_DGRAM, 0);
	if (sd < 0)
	{
		perror("socket() error\n");
		exit(1);
	}

	sin = (struct sockaddr_in *) &arpreq.arp_pa;
	memset(sin, 0, sizeof(struct sockaddr_in));
	sin->sin_family=AF_INET;
	ina.s_addr = inet_addr(ip);
	memcpy(&sin->sin_addr, (char *)&ina, sizeof(struct in_addr));

	eth_addr=ether_aton(eaddr);
	if(!eth_addr)
	{
		printf("Could not convert eth addr");
		return -1;
	}
	
	memcpy(arpreq.arp_ha.sa_data, eth_addr, sizeof(struct ether_addr));

	/*
	for(i=0;i<6;i++)
	{
		//arpreq.arp_ha.sa_data[i] = eth_addr->ether_addr_octet[i];
		printf("%x %x\n", (u_char)arpreq.arp_ha.sa_data[i], eth_addr->ether_addr_octet[i]);
	}
	*/

	arpreq.arp_flags=ATF_PERM;

	strcpy(arpreq.arp_dev, eth_dev);

	rc = ioctl(sd, SIOCSARP, &arpreq);
	if (rc < 0)
	{
		perror("SIOCSARP ERROR:");
	}
	else
	{
		printf("SIOCSARP SUCCESS\n");
	}
	return 0;
}
