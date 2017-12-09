
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "dbg.h"

void fexit1(void) {
    DBG("fexit1");
}
void fexit2(void) {
    DBG("fexit2");
}
void fexit3(void) {
    DBG("fexit3");
}
void fexit4(int v, void *p){
    DBG("fexit4 %d on %s", v, (char *)p);
}
char *str_arg = "forrest jiang";
int main(int argc, char *argv[]) {

    int ret_st =0;
    char *str;
    int len;

    int fd = open("test.txt", O_RDWR);
    if(fd == -1)
        FAIL("Open");

    int ret = fork();
    if(ret == -1) {
        FAIL("Exec fail");

    } else if (ret == 0) {
        ret_st = 10;
        DBG("====> child pid %jd ppid %jd", (intmax_t)getpid(), (intmax_t)getppid());
        if(lseek(fd, 10, SEEK_SET) == (off_t)-1)
            FAIL("Child Seek");
        str = "jiang";
        len = strlen(str);
        if(len != write(fd, str, len))
            FAIL("Child write");
        sleep(10);
        DBG("Child exit");
#if 0 // for core dump test
        char *s = NULL;
        *s = 1;
#endif

    } else {
        DBG("====> parent pid %jd ppid %jd", (intmax_t)getpid(), (intmax_t)getppid());
        if(lseek(fd, 100, SEEK_SET) == (off_t)-1)
            FAIL("Parent Seek");
        str = "forrest";
        len = strlen(str);
        if(len != write(fd, str, len))
            FAIL("Parent write");

        /**
         * will be called in the reverse order of registering
         */
        if(atexit(fexit1) != 0)
            FAIL("Reg 1");
        if(atexit(fexit2) != 0)
            FAIL("Reg 2");
        if(on_exit(fexit4, str_arg) != 0)
            FAIL("Reg 4");
        if(atexit(fexit3) != 0)
            FAIL("Reg 3");


        int st;
        pid_t pid = wait(&st);
        DBG("wait fork pid %d st %d", pid, st);
        if(ret != pid)
            DBG("Child ret pid %d mismatch", ret);

        if(WIFEXITED(st)) {
            DBG("Child exit normally with status %d", WEXITSTATUS(st));
        }
        else if(WIFSIGNALED(st)) {
            DBG("Child exit by signal with sig %d%s",
                    WTERMSIG(st),
                    WCOREDUMP(st) ? ", core dumped" : "");
        }
        else if(WIFSTOPPED(st)) {
            DBG("Child exit stopped by sig %d", WSTOPSIG(st));
        }
        else if(WIFCONTINUED(st)) {
            DBG("Child exit continued");
        }

    }



    close(fd);
    return ret_st;
}
