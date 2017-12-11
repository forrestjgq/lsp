#define _XOPEN_SOURCE

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "dbg.h"


int main(int argc, char *argv[]) {

    int ret = system("./test.sh fork");
    if(ret == -1)
        FAIL("system");
    else
        DBG("fork test done, status %d", WEXITSTATUS(ret));

    ret = system("./test.sh");
    if(ret == -1)
        FAIL("system 2");
    else
        DBG("all test done, status %d", WEXITSTATUS(ret));

    return 0;
}
