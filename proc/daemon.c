#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dbg.h"

/**
 * This test demonstrate the implementation of daemon()
 */
int main(int argc, char *argv[]) {

    pid_t pid = fork();

    if(pid == -1)
        FAIL("fork");

    if(pid != 0)
        exit(EXIT_SUCCESS);

    if(setsid() == -1)
        FAIL("set sid");

    if(chdir("/") == -1)
        FAIL("chdir");

    int i;
    for(i = 0; i < 3; i++)
        close(i);

    open("~/dlog", O_RDWR|O_CREAT, 0644);
    dup(0);
    dup(0);


    while(1) ;

    return 0;

}
