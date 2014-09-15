#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>

struct my_msg {
    int seq_no;
    int size;
    char data[];
};


int my_seq_no;
int packet_size = 4096;
pthread_spinlock_t seq_no_lock;

static int
get_my_seq_number() {

	int seq_no;
	pthread_spin_lock(&seq_no_lock);
	seq_no = my_seq_no++;
	pthread_spin_unlock(&seq_no_lock);

	return seq_no;
}

static int
verify_seq_number(int seq_no) {

	int rc = 0;
	pthread_spin_lock(&seq_no_lock);
	if (my_seq_no != seq_no) {
		rc = -1;
	}
	seq_no = my_seq_no + 1;
	pthread_spin_unlock(&seq_no_lock);
	return 0;
}

struct sock_args {
	int sock_fd;
	struct sockaddr_in *sock_addr;
};

static int
sender(int sock_fd, struct sockaddr_in *sock_addr) {

	int rc;

	struct my_msg *msg = malloc(sizeof(struct my_msg) + packet_size);
	msg->size = packet_size;
	msg->seq_no = get_my_seq_number();
	memset(msg->data, msg->seq_no, packet_size);
	rc = sendto(sock_fd, msg, packet_size + sizeof(struct my_msg), 
			0, (struct sockaddr*) sock_addr, sizeof(*sock_addr)); 
	printf("Tx: %d\n", msg->seq_no);
	return rc;
}

static void
print_err(char *msg) {
	printf("Error: %s errno %d\n", msg, errno);	
	perror("Error:");
}

static int
recver(int sock_fd, struct sockaddr_in *sock_addr) {

	char *buffer;
	int seq_no;
	struct my_msg *msg;
	struct sockaddr_in sock_addr_recv;
	int sock_addr_len;
	int rc;

	buffer = malloc(sizeof(struct my_msg) + packet_size);
	rc = recvfrom(sock_fd, buffer, sizeof(struct my_msg) + packet_size, 
			0, &sock_addr_recv, &sock_addr_len);
	if (rc < 0) {
		print_err("recver recvfrom failed");
		free(buffer);
		return rc;
	}

	if (rc != (sizeof(*msg) + packet_size)) {
		print_err("Bad msg");
		return -EMSGSIZE;
	}

	msg = (struct my_msg *)buffer;

	if ((rc = verify_seq_number(msg->seq_no)) < 0) {
		printf("Bad sequence number %d\n", msg->seq_no);
	}

	printf("Rx %d\n", msg->seq_no);

	return rc;
}


static void*
sender_thread(void *args) {
	int rc;
	struct sock_args *sargs = (struct sock_args *) args;
	while (1) {
		rc = sender(sargs->sock_fd, sargs->sock_addr);
		if (rc < 0) {
			print_err("sender error");
		}
	}
}

static void*
recver_thread(void *args) {
	int rc;
	struct sock_args *sargs = (struct sock_args *) args;
	while (1) {
		rc = recver(sargs->sock_fd, sargs->sock_addr);
		if (rc < 0) {
			print_err("sender error");
		}
	}
}

static int
echoer(int sock_fd) {

	char *buffer;
	int packet_len;
	int rc;
	int sock_len;
	struct sockaddr_in sock_addr;

	buffer = malloc(sizeof(struct my_msg) + packet_size);
	rc = recvfrom(sock_fd, buffer, sizeof(struct my_msg) + packet_size, 
			0, &sock_addr, &sock_len);
	if (rc < 0) {
		printf("echoer fd %d\n", sock_fd);
		print_err("echoer recvfrom failed");
		free(buffer);
		return rc;
	}

	rc = sendto(sock_fd, buffer, rc, 0, &sock_addr, sizeof(sock_addr));

	if (rc < 0) 
		print_err("echoer sendto failed");

	free(buffer);

	return rc;
}

#define MY_PORT (4455)

static void print_help() {
	printf("test_udp < echo_server | echo_client | echo_thread_client > [ server_ip ]\n");
}

int 
main(int argc, char *argv[]) {

	int rc;
	int sock_fd;
	struct sockaddr_in sock_addr;
	int echo_mode = 0;
	int thread_mode = 0;

	if (argc < 2) {
		print_help();
		return 0;
	}

	pthread_spin_init(&seq_no_lock, PTHREAD_PROCESS_SHARED);

	/* create the socket */
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd < 0) {
		print_err("sock create failed");
		return -1;
	}

	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_addr.sin_port = htons(MY_PORT);



	/* parse args */
	if (!strncmp(argv[1], "echo_server", strlen("echo_server"))) {
		echo_mode = 1;
	} else
	if (!strncmp(argv[1], "echo_client", strlen("echo_client"))) {
		echo_mode = 2;
	} else
	if (!strncmp(argv[1], "echo_thread_client", strlen("echo_thread_client"))) {
		echo_mode = 2; thread_mode = 1;
	} else {
		print_help();
		return 0;
	}


	/* set sock_addr according to echo_mode */
	if (echo_mode == 2) {
		if (argc < 3) {
			print_help();
			return 0;
		} else {
			if (!inet_aton(argv[2], &sock_addr.sin_addr)) {
				printf("Bad IP address %s\n", argv[2]);
				return -EINVAL;
			}
		}
	} else {
		if (bind(sock_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
			print_err("bind failed");
			return -1;
		}
	}


	if (echo_mode == 1) {
		/* echo server */
		while (1) {
			rc = echoer(sock_fd);
			if (rc < 0) {
				print_err("echoer failed");
				return rc;
			}
		}
	} else {
		/* echo client */
		struct sock_args sargs;
		sargs.sock_fd = sock_fd;
		sargs.sock_addr = &sock_addr;

		if (thread_mode) {
			pthread_t th;
			pthread_attr_t th_attr;
			pthread_attr_setdetachstate(&th_attr, 1);
			if (pthread_create(&th, &th_attr, sender_thread, &sargs)) {
				print_err("thread create for sender failed");
				return -1;
			}
			if (pthread_create(&th, &th_attr, recver_thread, &sargs)) {
				print_err("thread create for recver failed");
				return -1;
			}
		} else {
			while (1) {
				if ((rc = sender(sock_fd, &sock_addr)) < 0) {
					print_err("sender failed");
					return rc;
				}
				if ((rc = recver(sock_fd, &sock_addr)) < 0) {
					print_err("recver failed");
					return rc;
				}
			}
		} // thread_mode

	} // echo_mode

	return 0;

} //main



