#define _GNU_SOURCE
/**
 * tp:
 * - priority: nice() getpriority() setpriority()
 * - CPU affinity: sched_getaffinity(), sched_setaffinity()
 * - CPU utility: CPU_CLR, CPU_ZERO, CPU_ISSET, CPU_COUT
 *   sched_getcpu()
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sched.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>
#ifndef OVERFLOW
#include "dbg.h"
#else
#include <errno.h>
#include <assert.h>
#define DBG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define FAIL(str)\
    do { \
        if(errno)\
            perror("errno");\
        printf("fail @ %s:%d\n", __FILE__, __LINE__);\
        printf("\t%s\n", str);\
        return 1;\
    }while(0)
#define ENSURE assert
#endif

int nchild = 0;
#define MAX_CHILD 10
pid_t children[MAX_CHILD];
int64_t getts(void) {

    struct timespec tms;

    /* The C11 way */
    /* if (! timespec_get(&tms, TIME_UTC)) { */

    /* POSIX.1-2008 way */
    if (clock_gettime(CLOCK_REALTIME,&tms)) {
        return -1;
    }
    /* seconds, multiplied with 1 million */
    int64_t micros = tms.tv_sec * 1000000;
    /* Add full microseconds */
    micros += tms.tv_nsec/1000;
    /* round up if necessary */
    if (tms.tv_nsec % 1000 >= 500) {
        ++micros;
    }
    //printf("Microseconds: %"PRId64"\n",micros);
    return micros;
}
void exit_proc(void) {
    int i = 0;
    int st;
    for(i = 0; i < nchild; i++)
        waitpid(children[i], &st, 0);
}

void run(int seq) {
    int i = 0, j = 0;
    int cnt = 10;
    int max = 0x04ffffff;
    int num = 0;

    for(i = 0; i < cnt; i++) {
        DBG("MS %"PRId64" Run %d cnt %d", getts(), seq, i);
        for(j = 0; j < max; j++){
            num += j;
        }
    }
}
int fork_rt(int seq, int policy, int prio) {
    pid_t pid = fork();
    ENSURE(pid != -1);

    if(pid == 0) {
        DBG("child %d prio %d policy %d", seq, prio, policy);
        struct sched_param sp = { .sched_priority = prio };
        int ret = sched_setscheduler(0, policy, &sp);
        if(ret == -1) {
            perror("set sched");
            goto ENDFORK;
        }

        int policy1 = sched_getscheduler(0);
        if(policy1 == -1) {
            perror("get policy");
            goto ENDFORK;
        }

        memset (&sp, 0, sizeof(sp));
        if(sched_getparam(0, &sp) == -1) {
            perror("getparam");
            goto ENDFORK;
        }

        if(policy1 != policy || sp.sched_priority != prio)
            DBG("Seq %d policy %d != %d or prio %d != %d",
                    seq, policy1, policy, sp.sched_priority, prio);
    }

ENDFORK:
    return pid;
}
/**
 * This test will fork several child processes,
 * each child will be set with different RT scheduling types
 * and priorities.
 *
 * This test requires ROOT priviledage
 */
int main(int argc, char * argv[]) {

    int rr_min, rr_max;
    int fifo_min, fifo_max;

    rr_min = sched_get_priority_min(SCHED_RR);
    rr_max = sched_get_priority_max(SCHED_RR);
    fifo_min = sched_get_priority_min(SCHED_FIFO);
    fifo_max = sched_get_priority_max(SCHED_FIFO);

    DBG("Prio range RR(%d - %d) FIFO(%d - %d)",
            rr_min, rr_max, fifo_min, fifo_max);

    if(rr_min < 0 || rr_max < 0 ||
            fifo_min < 0 || fifo_max < 0){
        FAIL("Invalid priority range");
    }

    /**
     * Now fork to start 2 RR with same priorities
     */
    int prio = rr_max > fifo_max ? fifo_max : rr_max;
    /**
     * at least one lower priority for fifo
     */
    if(prio - 1 < fifo_min)
        FAIL("Fail to sched fifo");

    if (fork_rt(1, SCHED_RR, prio) == 0) {
        /** child */
        DBG("RR child 1 with prio %d running", prio);
        run(1);
    } else if(fork_rt(2, SCHED_RR, prio) == 0) {
        DBG("RR child 2 with prio %d running", prio);
        run(2);
    } else if(fork_rt(3, SCHED_FIFO, prio / 2) == 0) {
        DBG("FIFO child 3 running");
        run(3);
    } else {
        DBG("parent running");

        if(atexit(exit_proc) != 0)
            perror("atexit");
        run(0);
    }

    return 0;
}


