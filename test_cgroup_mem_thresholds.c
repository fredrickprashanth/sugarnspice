#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdint.h>
#include <sys/eventfd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <libgen.h>


static int
mem_alerts_init(char *memcg_dir, uint64_t total_mem) {                                                                   
  
    struct sysinfo si;
    int fd_event_control;
    char event_str[255];
    char event_control_file[255];
    char memusage_file[255];
    char *cg_dirname;
    uint64_t threshold_mem;
    int fd_mem_alert_minor;
    int fd_mem_alert_severe;
    int fd_mem_alert_critical;
    int fd_cgroup_all_mem_usage;
    int ret;

                                                                                      
    fd_mem_alert_minor = eventfd(0, 0);                                               
    fd_mem_alert_severe = eventfd(0, 0);                                              
    fd_mem_alert_critical = eventfd(0, 0);
                                                                                      
#define MINOR_THRESHOLD_PERCENT 	(50)
#define SEVERE_THRESHOLD_PERCENT 	(70)
#define CRITICAL_THRESHOLD_PERCENT 	(90)
  
    sprintf(memusage_file, "%s/memory.usage_in_bytes", memcg_dir);
    fd_cgroup_all_mem_usage = open(memusage_file, O_RDONLY);                        
    if (fd_cgroup_all_mem_usage < 0) {                                                
        printf("Error opening cgroup file %s err %d\n", memusage_file, errno);      
        return fd_cgroup_all_mem_usage;
    }

    snprintf(event_control_file, sizeof(event_control_file), "%s/cgroup.event_control",
             memcg_dir);

    fd_event_control = open(event_control_file, O_WRONLY);                      
    if (fd_event_control < 0) {
        printf("Error opening the control file %s err %d\n", event_control_file, errno);
        return fd_event_control;
    }
                                                                                      
    threshold_mem = total_mem*MINOR_THRESHOLD_PERCENT/100;
    printf("minor threshold = %llu\n", threshold_mem);
    snprintf(event_str, sizeof(event_str), "%d %d %llu", fd_mem_alert_minor,
	         fd_cgroup_all_mem_usage, threshold_mem);
    ret = write(fd_event_control, event_str, strlen(event_str) + 1);                        
    if (ret < 0) {
        printf("Error writing %s err %d\n", event_str, errno);
        return ret;
    }
    
    threshold_mem = total_mem*SEVERE_THRESHOLD_PERCENT/100;
    printf("severe threshold = %llu\n", threshold_mem);
    snprintf(event_str, sizeof(event_str), "%d %d %llu", fd_mem_alert_severe,
             fd_cgroup_all_mem_usage, threshold_mem);                                                                         
    ret = write(fd_event_control, event_str, strlen(event_str) + 1);                        
    if (ret < 0) {
        printf("Error writing %s err %d\n", event_str, errno);
        return ret;
    }
    
    threshold_mem = total_mem*CRITICAL_THRESHOLD_PERCENT/100;
    printf("critical threshold = %llu\n", threshold_mem);
    snprintf(event_str, sizeof(event_str), "%d %d %llu", fd_mem_alert_critical,
             fd_cgroup_all_mem_usage, threshold_mem);
    ret = write(fd_event_control, event_str, strlen(event_str) + 1);                        
    if (ret < 0) {
        printf("Error writing %s err %d\n", event_str, errno);
        return ret;
    }
    

    int max_fd = 0;
    fd_set readfds;

    FD_ZERO(&readfds);

    FD_SET(fd_mem_alert_minor, &readfds);
    max_fd = fd_mem_alert_minor > max_fd ? fd_mem_alert_minor : max_fd;

    FD_SET(fd_mem_alert_severe, &readfds);
    max_fd = fd_mem_alert_severe > max_fd ? fd_mem_alert_severe : max_fd;

    FD_SET(fd_mem_alert_critical, &readfds);
    max_fd = fd_mem_alert_critical > max_fd ? fd_mem_alert_critical: max_fd;

    int64_t eventfd_val;
    while ((ret = select(max_fd + 1, &readfds, NULL, NULL, NULL)) >= 0 ) {
        if (FD_ISSET(fd_mem_alert_minor, &readfds)) {
            printf(">> MINOR ALERT OCCURRED <<\n");
            if ((ret = read(fd_mem_alert_minor, &eventfd_val,
                            sizeof(eventfd_val)) < 0)) {
                printf("reading minor alert eventfd failed with %d\n", errno);
                return ret;
            } 
            printf("minor event count = %lld\n", eventfd_val);
        }
        if (FD_ISSET(fd_mem_alert_severe, &readfds)) {
            printf(">> SEVERE ALERT OCCURRED <<\n");
            if ((ret = read(fd_mem_alert_severe, &eventfd_val,
                            sizeof(eventfd_val)) < 0)) {
                printf("reading severe alert eventfd failed with %d\n", errno);
                return ret;
            } 
            printf("alert event count = %lld\n", eventfd_val);
        }
        if (FD_ISSET(fd_mem_alert_critical, &readfds)) {
            printf(">> CRITICAL ALERT OCCURRED <<\n");
            if ((ret = read(fd_mem_alert_critical, &eventfd_val,
                            sizeof(eventfd_val)) < 0)) {
                printf("reading critical alert eventfd failed with %d\n", errno);
                return ret;
            } 
            printf("critical event count = %lld\n", eventfd_val);
        }

        FD_SET(fd_mem_alert_minor, &readfds);
        FD_SET(fd_mem_alert_severe, &readfds);
        FD_SET(fd_mem_alert_critical, &readfds);
    }

    return ret;
}                             

#define NARGS (2)

int
main(int argc, char *argv[]) {

    int ret;
    uint64_t total_mem;
    struct sysinfo si;

    if (argc < (NARGS+1)) {
        printf("test_mem_alerts <memcg> <limit_bytes>\n");
        return 0;
    }

    if ((ret = sysinfo(&si)) < 0) {
        printf("Error getting sysinfo\n");
        return ret;
    }
    sscanf(argv[2], "%llu", &total_mem);

    if (mem_alerts_init(argv[1], total_mem) != 0) {
        printf("failed\n");
    }
    return 0;
}


