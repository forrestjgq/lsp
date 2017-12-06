
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include "dbg.h"

#define MAX_ARGS 20
int main(int argc, char *argv[]) {

    if(argc < 3 || argc > MAX_ARGS ) {
        PERR("Usage: %s <target_exec_file> <target_exec_name>, <param>...", argv[0]);
        return 1;
    }

    /**
     * tp: uid/pid/ppid/gid won't change through execvp()
     */

    PERR("Program: %s", argv[0]);

    PERR("====> pid %jd ppid %jd", (intmax_t)getpid(), (intmax_t)getppid());
    PERR("====> uid %jd euid %jd", (intmax_t)getuid(), (intmax_t)geteuid());

#if 0
    /**
     * code snippet from lsp book page 143
     * and it is incorrect.
     * A compile error will occur
     */
    if(execvp("vi", "vi", "./execvp.c", NULL) == -1) {
        perror("execvp");
        PERR("Fail to exec: %s %s", argv[1], argv[2]);
        return 2;
    }

#endif
    DBG("Now execvp without full path shall be succeed");
    SEPERATE();

    char *v[MAX_ARGS] = {
        argv[2],
        NULL
    };

    int i, j;
    for(i = 2, j = 0; i < argc; i++, j++)
        v[j] = argv[i];
    v[j] = NULL;

    if(execvp(argv[1], v) == -1) {
        perror("execvp");
        PERR("Fail to exec: %s %s", argv[1], argv[2]);
        return 2;
    }

    DBG("Done");
    return 0;
}
