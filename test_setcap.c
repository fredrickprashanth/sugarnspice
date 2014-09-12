#include <sys/types.h>
#include <sys/capability.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int
dump_smaps(int pid) {
    char file_name[255];
    int fd;
    char buf[255];
    int ret;
    snprintf(file_name, sizeof(file_name), "/proc/%d/smaps", pid);
    fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        perror("Open:");
        return fd;
    }
    memset(buf, 0, sizeof(buf));
    while ((ret = read(fd, buf, sizeof(buf))) > 0) {
        buf[ret] = 0;
        printf("%s", buf);
    }
    if (ret < 0) {
        perror("read:");
    }
    close(fd);
    return 0;
}

        
    
#define NARGS 1
int main(int argc, char *argv[])
{
    int pid;
    int ret;
    cap_t caps;
    caps = cap_get_proc();
    cap_value_t cap_list[2];

    if (argc < (NARGS+1)) {
        printf("test_set_cap <pid>\n");
        return 0;
    }

    sscanf(argv[1], "%d", &pid);

#if 0

    cap_list[0] = CAP_SETUID;
    cap_list[1] = CAP_SETGID;

    cap_set_flag(caps, CAP_EFFECTIVE, 2, cap_list, CAP_SET);
    cap_set_flag(caps, CAP_INHERITABLE, 2, cap_list, CAP_SET);
#endif
    cap_list[0] = CAP_SYS_PTRACE;
    cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, CAP_SET);
    cap_set_flag(caps, CAP_INHERITABLE, 1, cap_list, CAP_SET);

    ret = cap_set_proc(caps);
    if (ret < 0) {
        perror("cap_set_proc:");
        return ret;
    }
    cap_free(caps);
#if 0
    setgid(65534);
    setuid(65534);
#endif 

    cap_get_proc();

    printf("Dumping smaps of pid %d\n", pid);
    dump_smaps(pid);

#if 0
    setgid(0);
    setuid(0);
#endif

    return 0;
}
