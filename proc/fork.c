
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

    int ret = fork();
    if(ret == -1) {
        FAIL("Exec fail");
    } else if (ret == 0) {
        DBG("Parent");
    } else {
        DBG("Child");
    }

    PERR("====> pid %jd ppid %jd", (intmax_t)getpid(), (intmax_t)getppid());
    return 0;
}
