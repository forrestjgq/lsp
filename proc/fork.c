
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "dbg.h"


int main(int argc, char *argv[]) {

    char *str;
    int len;

    int fd = open("test.txt", O_RDWR);
    if(fd == -1)
        FAIL("Open");

    int ret = fork();
    if(ret == -1) {
        FAIL("Exec fail");
    } else if (ret == 0) {
        DBG("Parent");
        if(lseek(fd, 100, SEEK_SET) == (off_t)-1)
            FAIL("Parent Seek");
        str = "forrest";
        len = strlen(str);
        if(len != write(fd, str, len))
            FAIL("Parent write");
    } else {
        DBG("Child");
        if(lseek(fd, 10, SEEK_SET) == (off_t)-1)
            FAIL("Child Seek");
        str = "jiang";
        len = strlen(str);
        if(len != write(fd, str, len))
            FAIL("Child write");
    }

    PERR("====> pid %jd ppid %jd", (intmax_t)getpid(), (intmax_t)getppid());


    close(fd);
    return 0;
}
