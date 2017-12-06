
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include "dbg.h"

/**
 * This example demostrate user id and group id
 */
int main(int argc, char *argv[]) {

    if(argc < 3) {
        PERR("Usage: %s <target_exec_file> <target_exec_name>", argv[0]);
        return 1;
    }

    /**
     * tp: uid/pid/ppid/gid won't change through execl()
     */

    PERR("Program: %s", argv[0]);

    PERR("====> pid %jd ppid %jd", (intmax_t)getpid(), (intmax_t)getppid());
    PERR("====> uid %jd euid %jd", (intmax_t)getuid(), (intmax_t)geteuid());

    DBG("execl without full path, shall be failed");

    SEPERATE();

    if(execl(argv[1], argv[2], NULL) == -1) {
        perror("execl just as expected");
        PERR("execl fail to exec: %s %s", argv[1], argv[2]);
    }

    DBG("Now execlp without full path shall be succeed");
    SEPERATE();

    if(execlp(argv[1], argv[2], NULL) == -1) {
        perror("execlp");
        PERR("Fail to exec: %s %s", argv[1], argv[2]);
        return 2;
    }

    DBG("Done");
    return 0;
}
