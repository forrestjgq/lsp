#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE
/**
 * tp:
 * - compare the sid with bash
 * - get group id
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h> // for intmax_t
#include <sys/wait.h>
#include "dbg.h"

char strind[2];
int nrchild = 0;
pid_t start_fork(char *cmd, char *arg) {
    pid_t pid = fork();

    if(pid == -1)
        FAIL("Fail to fork");


    if(pid == 0) {
        execl(cmd, "./session", arg, NULL);
        FAIL("execl fail");
    }

    nrchild ++;

    return pid;
}
void wait_child(void) {
    int st;
    while(nrchild) {
        wait(&st);
        --nrchild;
    }
}
void sess_exit(int status, void *arg) {
    int ind = (int)(long)arg;
    DBG("Proc %d exit", ind);
}
/**
 *
 * session bash      +---> Process 2(new session)
 *       +           |               +
 *       |           |               |
 *       |           |               |
 *       v           |               v
 *   Process 0 +-----+           Process 4
 *       +
 *       |
 *       v
 *   Process 1 +----------->Process 3(new group)
 *                                  +
 *                                  |
 *                                  |
 *                                  v
 *                                Process 5
 */
int main(int argc, char * argv[]) {

    int ind = 0;
    if(argc != 1) {
        ind = atoi(argv[1]);
    }

    if(on_exit(sess_exit, (void *)(long)ind))
        FAIL("onexit");

    if(ind == 0) {
        /**
         * entry of test
         *
         * create a process in the same session and same group
         */
        start_fork(argv[0], "1");
        start_fork(argv[0], "2");
        /**
         * delay 1s to let all process call session executable
         * in case it is removed after process 0 done
         */
        sleep(1);
    } else if (ind == 1) {
        start_fork(argv[0], "3");
    } else if (ind == 2) {
        pid_t sid = setsid();
        if(sid == -1)
            FAIL("set sid");
        start_fork(argv[0], "4");
    } else if (ind == 3) {
        pid_t pgid = setpgid(0, 0);
        if(pgid == -1)
            FAIL("Set pgid");
        start_fork(argv[0], "5");
    }


    pid_t pid = getpid();
    PERR("Process %d has pid %jd sid %jd pgid %jd",
            ind,
            (intmax_t)pid,
            (intmax_t)getsid(pid),
            (intmax_t)getpgid(pid));


    wait_child();
    return 0;
}


