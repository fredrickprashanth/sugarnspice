#include <stdio.h>
#include <stdint.h>
#include <string.h>

long unsigned int cpukhz;
#define CPU_MHZ_STR "cpu MHz"
#define CPU_INFO_STR_LEN_MAX 100
#define CPU_INFO_FS_NAME "vmware_cpuinfo"

int main()
{
    FILE *fd;
    char cpu_info_str[CPU_INFO_STR_LEN_MAX], *offset_p;
    int found = 0;
    unsigned int i;
    int rc;
    
    fd = fopen(CPU_INFO_FS_NAME, "r");
    if (!fd) {
        return(rc);
    }
    
    while (fgets(cpu_info_str, CPU_INFO_STR_LEN_MAX, fd) > 0) {
        if (strncmp(cpu_info_str, CPU_MHZ_STR, strlen(CPU_MHZ_STR)) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        fclose(fd);
        return(rc);
    }

    offset_p = cpu_info_str;
    for (i=0; i<CPU_INFO_STR_LEN_MAX; i++) {
        if (*offset_p++ == ':') {
            float mhz;
            sscanf(offset_p, "%f", &mhz);
            cpukhz = (mhz * 1000);

            rc = 0;
            break;
        }
    }
    


    fclose(fd);


    printf("cpukhz = %lu\n", cpukhz);
    return(rc);
}

