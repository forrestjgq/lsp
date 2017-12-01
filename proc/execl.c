
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>


#define DBG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define FAIL(str)\
    do { \
        if(errno)\
            perror("errno");\
        printf("fail @ %s:%d\n", __FILE__, __LINE__);\
        printf("\t%s\n", str);\
        return 1;\
    }while(0)

#define PERR(fmt, ...) dprintf(STDOUT_FILENO, fmt "\n", ##__VA_ARGS__)
/**
 * This example demostrate user id and group id
 */
int main(int argc, char *argv[]) {

    if(argc < 3) {
        PERR("Usage: %s <target_exec_path> <target_exec_name>", argv[0]);
        return 1;
    }

    /**
     * tp: uid/pid/ppid/gid won't change through execl()
     */

    PERR("Program: %s", argv[0]);

    PERR("====> pid %jd ppid %jd", (intmax_t)getpid(), (intmax_t)getppid());
    PERR("====> uid %jd euid %jd", (intmax_t)getuid(), (intmax_t)geteuid());

    if(execl(argv[1], argv[2], NULL) == -1) {
        perror("execl");
        PERR("Fail to exec: %s %s", argv[1], argv[2]);
        return 2;
    }

    return 0;
}
