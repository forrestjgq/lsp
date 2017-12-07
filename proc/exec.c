
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

enum tname {
    EXECL,
    EXECLE,
    EXECLP,
    EXECV,
    EXECVE,
    EXECVP,

    EXECMAX
} ;

char *strname [ EXECMAX ] = {
    "execl",
    "execle",
    "execlp",
    "execv",
    "execve",
    "execvp"
};

#define MOD_NAME "exec"
int main(int argc, char *argv[]) {

    if (argc != 2)
        FAIL("Invalid argc");

    SEPERATE();
    PERR("Program: %s", argv[0]);

    char *val = getenv(ENV_KEY);
    if(val == NULL)
        DBG("No env of %s", ENV_KEY);
    else {
        DBG("Env %s: %s", ENV_KEY, val);
        if(strcmp(val, ENV_VAL) != 0)
            FAIL("Invalid env");
    }

    int state = atoi(argv[1]);
    if(state < 0 || state > EXECMAX)
        FAIL("Invalid arg");
    else if(state == EXECMAX) {
        SEPERATE();
        DBG("All test passed");
        return 0;
    }


    char *env[] = {
        ENV_KEY "=" ENV_VAL,
        NULL
    };

    char str[2];
    str[0] = state + 1 + '0';
    str[1] = 0;

    char *v[MAX_ARGS] = {
        strname[state],
        str,
        NULL
    };


    int ret;
    switch(state) {
        case EXECL:
            ret = execl("./" MOD_NAME, strname[state], str, NULL);
            break;
        case EXECLE:
            ret = execle("./" MOD_NAME, strname[state], str, NULL, env);
            break;
        case EXECLP:
            ret = execlp(MOD_NAME, strname[state], str, NULL);
            break;
        case EXECV:
            ret = execv("./" MOD_NAME, v);
            break;
        case EXECVE:
            ret = execve("./" MOD_NAME, v, env);
            break;
        case EXECVP:
            ret = execvp(MOD_NAME, v);
            break;
        default:
            FAIL("Invalid state");
    }

    if(ret == -1) {
        FAIL("Exec fail");
    }

    return 0;
}
