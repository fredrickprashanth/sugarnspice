#include <linux/fanotify.h>

int main(void) {
	int fan_fd, len;
	struct fanotify_addr addr;
	socklen_t socklen;
	char buf[4096];
	struct fanotify_event_metadata *metadata;
        
        memset(&addr, 0, sizeof(addr));
        addr.family = AF_FANOTIFY;
        addr.group_num = 123456;
        addr.priority = 32768;
        addr.mask = FAN_OPEN | FAN_GLOBAL_LISTENER;

        fan_fd = socket(PF_FANOTIFY, SOCK_RAW, 0);
	bind(fan_fd, (struct sockaddr *)&addr, sizeof(addr));
	while (1) {
		socklen = sizeof(buf);
		getsockopt(fan_fd, SOL_FANOTIFY, FANOTIFY_GET_EVENT,
			   buf, &socklen);
		metadata = &buf;
		len = socklen;
		while(FAN_EVENT_OK(metadata, len)) {
			printf("got event!\n"
			close(metadata->fd);
			metadata = FAN_EVENT_NEXT(metadata, len);
		}
	}
}

