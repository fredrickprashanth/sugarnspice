#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int 
main() {

    int pipe_fds[2];
    int ret;
    char buff[4096], ch = 'a';
    struct timeval t1, t2;

    ret = pipe(pipe_fds);

    if (ret < 0) {
        printf("pipe creation failed with error %d\n", errno);
        return ret;
    }

    if (fork()) {
        while (1) {
            sleep(1);
            ret = read(pipe_fds[0], buff, sizeof(buff));
            if (ret < 0) {
                printf("Read from pipe failed with error %d\n", errno);
                return ret;
            }
            printf("Read %d bytes from pipe\n", ret);
        }
    } else {
        while (1) {
            memset(buff, ch++, sizeof(buff)); 
            if ((ret = write(pipe_fds[1], buff, sizeof(buff))) < 0) {
                printf("Write to pipe failed with error %d\n", errno);
                return ret;
            }
            printf("Wrote %d bytes to pipe\n", ret);
        }
    }
        
    return 0;
}
