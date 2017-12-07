
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

#define MAX_ARGS 20
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
     * tp: uid/pid/ppid/gid won't change through execve()
     */

    PERR("Program: %s", argv[0]);
    SEPERATE();


    if(more) {
        char *env[] = {
            ENV_KEY "=" ENV_VAL,
            NULL
        };
        char *v[MAX_ARGS] = {
            argv[0],
            NULL
        };

        int i, j;
        for(i = 0, j = 0; i < argc; i++, j++)
            v[j] = argv[i];
        v[j] = NULL;

        if(execve(argv[0], v, env) == -1) {
            perror("execve");
            PERR("Fail to exec: %s %s", argv[1], argv[2]);
            return 2;
        }
    }

    DBG("Done");
    return 0;
}
