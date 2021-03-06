
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

#define ENV_KEY  "forrest"
#define ENV_VAL  "jiang"
/**
 * First execle will take nothing in
 * then execle with this executable and env
 */
int main(int argc, char *argv[]) {

    int more = 1;
    char *val = getenv(ENV_KEY);
    if(val == NULL)
        DBG("No env of %s", ENV_KEY);
    else {
        more = 0;
        DBG("Env %s: %s", ENV_KEY, val);
        if(strcmp(val, ENV_VAL) != 0)
            FAIL("Invalid env");
    }

    /**
     * tp: uid/pid/ppid/gid won't change through execle()
     */

    PERR("Program: %s", argv[0]);

    PERR("====> pid %jd ppid %jd", (intmax_t)getpid(), (intmax_t)getppid());
    PERR("====> uid %jd euid %jd", (intmax_t)getuid(), (intmax_t)geteuid());

    if(more) {
        char *env[] = {
            ENV_KEY "=" ENV_VAL,
            NULL
        };
        if(execle(argv[0], argv[0], NULL, env) == -1) {
            perror("execle");
            PERR("Fail to exec");
            return 2;
        }
    }

    return 0;
}
