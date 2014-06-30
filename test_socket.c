#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>

int64_t pkts_count = 0;
int64_t bytes_count = 0;
struct timespec start_ts;


void
sigalrm_hdlr(int sig) {
	 
	struct timespec curr_ts;
	clock_gettime(CLOCK_MONOTONIC, &curr_ts);

	int ts_diff = curr_ts.tv_sec - start_ts.tv_sec;
	printf("Pkts           : %lld\n", pkts_count);
	printf("Bytes          : %lld\n", bytes_count);
	printf("Pkts Rate(pps) : %lld\n" , ts_diff > 0 ? pkts_count/ts_diff : 0);
	printf("B/w(Bps)       : %lld\n" , ts_diff > 0 ? bytes_count/ts_diff : 0);
	clock_gettime(CLOCK_MONOTONIC, &start_ts);
	bytes_count = 0;
	pkts_count = 0;
	alarm(1);
}


#define MAX_CONNECTIONS 1
#define PORT		"99010"

static inline void print_help() {
	printf("test_socket [ -l ] | [ -s <server_ip]\n");
	exit(1);
}

static inline void change_sched() {

	int ret;
	struct sched_param param;
	param.sched_priority = 1;
	ret = sched_setscheduler(0, SCHED_FIFO, &param);
	if (ret < 0) {
		perror("sched_setscheduler");
	}
}

int
main(int argc, char *argv[]) {

	struct addrinfo hints;
	struct addrinfo *hostinfo;
	char ipstr[INET6_ADDRSTRLEN];
	int ret;
	int server_mode;
	int bytes_in;
	int client_sock_fd, sock_fd;
	char pkt_buf[256];
	char *host = NULL;

	memset(pkt_buf, 0xaa, sizeof(pkt_buf));
	memset(&hints, 0, sizeof(hints));
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

#define NARGS 1
/*
 * test_socket [-l] [-s <server_ip>] 
 * 
 * To run server
 * test_socket -l
 * To run client
 * test_socket -s <ip>
 *
 */
	if (argc >= (NARGS+1)) {
		if (!strcmp(argv[1], "-l")) {
			server_mode = 1;
		} else {
			if (argc < (NARGS+2)) {
				print_help();
			} else {
				if (!strcmp(argv[1], "-s")) {
					host = argv[2];
				} else {
					print_help();
				}
			}
		}
	} else {
		print_help();
	}

	if ((ret = getaddrinfo(host, PORT, &hints, &hostinfo)) != 0) {
		perror("getaddrinfo failed");
		printf("getaddrinfo error = %s\n", gai_strerror(ret));
		return ret;
	}

	assert(hostinfo != NULL);

	sock_fd = socket(hostinfo->ai_family, hostinfo->ai_socktype, hostinfo->ai_protocol);
	if (sock_fd < 0) {
		perror("socket");
		return sock_fd;
	}


	server_mode = 0;
	if (argc > 1) {
		if (!strcmp(argv[1], "-l")) {
			server_mode = 1;
		}
	}

	signal(SIGALRM, sigalrm_hdlr);
	alarm(2);

	//change_sched();
	if (server_mode) { 

		int yes = 1;
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		if (ret < 0 ){
			perror("setsockopt");
			return ret;
		}
		ret = bind(sock_fd, hostinfo->ai_addr, hostinfo->ai_addrlen);
		if (ret < 0) {
			perror("bind");
			return ret;
		}

		ret = listen(sock_fd, MAX_CONNECTIONS);
		if (ret < 0) {
			perror("listen");
			return ret;
		}

		socklen_t addr_size;
		struct sockaddr_storage client_addr;
		addr_size = sizeof(client_addr);

		client_sock_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addr_size);
	
		if (client_sock_fd < 0) {
			perror("accept");
			return client_sock_fd;
		}

		freeaddrinfo(hostinfo);

		clock_gettime(CLOCK_MONOTONIC, &start_ts);
		while (1) {
			bytes_in = 0;
			do {
				ret = send(client_sock_fd, pkt_buf + bytes_in, sizeof(pkt_buf) - bytes_in, 0);
				if (ret < 0) break;
				bytes_in += ret;
				bytes_count += ret;
			} while(ret > 0 && bytes_in < sizeof(pkt_buf));
			if (ret < 0) {
				perror("send");
				break;
			}
			pkts_count++;
		}
	}else {

		ret = connect(sock_fd, hostinfo->ai_addr, hostinfo->ai_addrlen);
		if (ret < 0) {
			perror("connect");
			return ret;
		}
		freeaddrinfo(hostinfo);

		clock_gettime(CLOCK_MONOTONIC, &start_ts);
		while(1) {
			bytes_in = 0;
			do {
				ret = recv(sock_fd, pkt_buf + bytes_in, sizeof(pkt_buf) - bytes_in, 0);
				if (ret < 0) break;
				bytes_in += ret;
				bytes_count += ret;
			} while(ret > 0 && bytes_in < sizeof(pkt_buf));
			if (ret < 0) {
				perror("recv");
				return ret;
			}
			pkts_count++;
		}
	}


	return 0;

}

