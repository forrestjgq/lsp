
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
#include "dbg.h"

int nchild = 0;
#define MAX_CHILD 10
pid_t children[MAX_CHILD];

void exit_proc(void) {
    int i = 0;
    int st;
    for(i = 0; i < nchild; i++)
        waitpid(children[i], &st, 0);
}

void run(int seq) {
    int i = 0, j = 0;
    int cnt = 20;
    int max = 0x0fffffff;
    int num = 0;

    for(i = 0; i < cnt; i++) {
        DBG("Run %d cnt %d", seq, i);
        for(j = 0; j < max; j++){
            num += j;
        }
    }
}
int fork_rt(int seq, int policy, int prio) {
    pid_t pid = fork();
    ENSURE(pid != -1);

    if(pid == 0) {
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
        DBG("FIFO child 1 running");
        run(1);
    } else if(fork_rt(2, SCHED_RR, prio) == 0) {
        DBG("FIFO child 2 running");
        run(2);
    } else if(fork_rt(3, SCHED_FIFO, prio - 1) == 0) {
        DBG("FIFO child 3 runninb");
        run(3);
    } else {
        DBG("parent runninb");

        if(atexit(exit_proc) != 0)
            perror("atexit");
        run(0);
    }

    return 0;
}


