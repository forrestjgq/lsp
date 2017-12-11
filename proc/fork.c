#define _DEFAULT_SOURCE

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "dbg.h"

static int bParent = 1;
void parent_exit(int v, void *p){
    DBG("parent exit %d on %s", v, (char *)p);
}

/**
 * test wait4 for NULL ptr write(SIGSEGV)
 */
int p_wait_segv(pid_t pid) {
    int status;
    struct rusage rs;

    pid_t c_pid = wait4(pid, &status, 0, &rs);
    if(c_pid == -1)
        FAIL("wait child fail");
    else {
        DBG("parent waited st %d(0x%08X)", status, status);
        if(pid != c_pid) {
            DBG("pid %d mismatch waited %d", pid, c_pid);
            FAIL("pid mismatch");
        }

        ENSURE(WIFSIGNALED(status));
        DBG("Term sig %s", strsig(WTERMSIG(status)));
        ENSURE(WCOREDUMP(status));
    }

    return 0;
}
int c_wait_segv(void) {
    char *s = NULL;
    *s = 0;
    return 0;
}
/**
 * test wait3 for divide-0 (SIGFPE)
 */
int p_wait_fpg(pid_t pid) {
    int status;
    struct rusage rs;

    pid_t c_pid = wait3(&status, 0, &rs);
    if(c_pid == -1)
        FAIL("wait child fail");
    else {
        DBG("parent waited st %d(0x%08X)", status, status);
        if(pid != c_pid) {
            DBG("pid %d mismatch waited %d", pid, c_pid);
            FAIL("pid mismatch");
        }

        ENSURE(WIFSIGNALED(status));
        DBG("Term sig %s", strsig(WTERMSIG(status)));
        ENSURE(WCOREDUMP(status));
    }

    return 0;
}
int c_wait_fpg(void) {

    int a = 19;
    int b = 0;
    int c = a/b;
    DBG("c = %d", c);
    return 0;
}
/**
 * test waitid for assert/abort(SIGABT)
 */
int p_wait_coredump(pid_t pid) {
    int status;
    siginfo_t si;

    int ret = waitid(P_PID, pid, &si,  WEXITED);

    if(ret == -1)
        FAIL("wait child fail");
    else {
        if(si.si_pid != pid ) {
            DBG("pid %d mismatch waited %d", pid, si.si_pid);
            FAIL("pid mismatch");
        }
        status = si.si_status;
        DBG("parent waited st %d(0x%08X)", status, status);
        ENSURE(si.si_code == CLD_DUMPED);

        ENSURE(WIFSIGNALED(status));
        DBG("Term sig %s", strsig(WTERMSIG(status)));
        /**
         * core dump flag is not included in si.status
         * but si.si_code can be used to get this indication
         */
        /*ENSURE(WCOREDUMP(status));*/
    }

    return 0;
}
int c_wait_coredump(void) {

    ENSURE(0);
    return 0;
}
/**
 * test waitpid for termination(SIGTERM)
 */
int p_wait_sigterm(pid_t pid) {
    int status;

    pid_t c_pid = waitpid(-1, &status, WNOHANG);
    ENSURE(c_pid == 0);

    kill(pid, SIGTERM);

    sleep(1);
    c_pid = waitpid(-1, &status, WNOHANG);
    if(c_pid == -1)
        FAIL("wait child fail");
    else {
        DBG("parent waited st %d(0x%08X)", status, status);
        if(pid != c_pid) {
            DBG("pid %d mismatch waited %d", pid, c_pid);
            FAIL("pid mismatch");
        }

        ENSURE(WIFSIGNALED(status));
        ENSURE(WTERMSIG(status) == SIGTERM);
    }

    return 0;
}
int c_wait_sigterm(void) {

    /**
     * Child will exit by SIGTERM even in sleep
     */
    sleep(10);
    return 0;
}
/**
 * test wait for failed normal exit
 */
int p_wait_normal_fail(pid_t pid) {
    int status;

    pid_t c_pid = wait(&status);
    if(c_pid == -1)
        FAIL("wait child fail");
    else {
        DBG("parent waited st %d(0x%08X)", status, status);
        if(pid != c_pid) {
            DBG("pid %d mismatch waited %d", pid, c_pid);
            FAIL("pid mismatch");
        }

        ENSURE(WIFEXITED(status));
        ENSURE(WEXITSTATUS(status) == 1);
    }

    return 0;
}
int c_wait_normal_fail(void) {

    return 1;
}

/**
 * test wait for succeed normal exit
 */
int p_wait_normal_succeed(pid_t pid) {
    int status;

    pid_t c_pid = wait(&status);
    if(c_pid == -1)
        FAIL("wait child fail");
    else {
        if(pid != c_pid) {
            DBG("pid %d mismatch waited %d", pid, c_pid);
            FAIL("pid mismatch");
        }

        ENSURE(WIFEXITED(status));
        ENSURE(WEXITSTATUS(status) == 0);
    }

    return 0;
}
int c_wait_normal_succeed(void) {

    return 0;
}

#define START_FORK(case) \
    do {\
        int ret = start_fork("test_"#case, p_##case, c_##case);\
        if(!bParent)\
            return ret;\
        else if(ret)\
            FAIL("Test failed");\
    }while(0)

void child_exit(void) {
    DBG("Child %jd exit", (intmax_t)getpid());
}
int start_fork(const char * strcase, int (*parent)(pid_t pid), int (*child)(void)) {
    pid_t pid;
    int ret = 0;

    DBG("=====================================");
    DBG("Start case %s", strcase);

    pid = fork();
    if(pid == -1)
        FAIL("Forking");
    else if(pid == 0) {
        bParent = 0;

        if(child) ret = child();
        DBG("Child ret %d", ret);

        if(atexit(child_exit) != 0)
            FAIL("child atexit fail");
    } else {
        sleep(1);
        if(parent) ret = parent(pid);
    }

    return ret;
}

int main(int argc, char *argv[]) {

    START_FORK(wait_normal_succeed);
    START_FORK(wait_normal_fail);
    START_FORK(wait_sigterm);
    START_FORK(wait_coredump);
    START_FORK(wait_fpg);
    START_FORK(wait_segv);

    if(bParent && on_exit(parent_exit, "Jiang") != 0)
        FAIL("Parent exit");

    return 0;
}
