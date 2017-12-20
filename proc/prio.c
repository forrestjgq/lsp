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
#include <unistd.h>
#include <sched.h>
#include <sys/resource.h>
#include <stdlib.h>
#include "dbg.h"

void show_affinity(cpu_set_t *pset) {
    pid_t pid =getpid();
    CPU_ZERO(pset);

    DBG("Current running @ %d", sched_getcpu());

    int ret = sched_getaffinity(pid, sizeof(cpu_set_t), pset);
    if(ret == -1)
        perror("Get affinity");
    else {
        int i;
        for( i = 0; i < CPU_SETSIZE; i++ ) {
            if(CPU_ISSET(i, pset))
                DBG("\tcpu %d is %s",
                    i, CPU_ISSET(i, pset) ? "set" : "unset");
        }
    }
}
void set_affinity(cpu_set_t *pset) {
    pid_t pid =getpid();
    int ret = sched_setaffinity(pid, sizeof(cpu_set_t), pset);
    if(ret == -1)
        perror("fail to set cpu");
    else
        show_affinity(pset);
}
/**
 * Adjust cpu affinity step by step until clear all cpu
 * affinity(will make an error of couse)
 */
int t_affinity(void) {
    /**
     * Here MUST NOT use struct cpu_set_t which yields a compiling
     * error of unknown storage size  of cpu_set_t
     */
    cpu_set_t iset;
    int i;

    CPU_ZERO(&iset);

    DBG("\nStart test affinity");
    show_affinity(&iset);

    int cnt = CPU_COUNT(&iset);

    DBG("\nClr harf cpu ");
    for(i = 0; i < cnt/2; i++)
        CPU_CLR(i, &iset);

    set_affinity(&iset);

    DBG("\nClr all cpu but one");
    for(i = 0; i < cnt - 1; i++)
        CPU_CLR(i, &iset);

    set_affinity(&iset);

    DBG("\nClr all cpu");
    for(i = 0; i < cnt; i++)
        CPU_CLR(i, &iset);

    set_affinity(&iset);

    return 0;
}
/**
 * priority get and set
 */
int t_prio(void){

    errno = 0;
    int curr = nice(0);

    if(errno != 0)
        perror("nice get fail");
    else
        DBG("curr nice %d", curr);

    errno = 0;
    int n = nice(-10);

    if(errno != 0)
        perror("nice -10 fail");
    else
        DBG("nice increase to %d", n);


    errno = 0;
    int prio = getpriority(PRIO_PROCESS, 0);
    if(errno)
        perror("get prio fail");
    else
        DBG("prio %d", prio);

    if(setpriority(PRIO_PROCESS, 0, prio+3) == -1)
        perror("set prio fail");
    else {
        errno = 0;
        int prio1 = getpriority(PRIO_PROCESS, 0);
        if(errno)
            perror("get prio fail");
        else if(prio1 != prio + 4)
            DBG("Set prio mismatch %d", prio1);
        else
            DBG("prio1 %d", prio1);
    }



    return 0;

}

int main(int argc, char * argv[]) {
    (void)t_prio();
    (void)t_affinity();

    return 0;
}


