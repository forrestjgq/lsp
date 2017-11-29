
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>

#define FAIL(str)\
    do { \
        perror("errno");\
        printf("fail @ %s:%d\n", __FILE__, __LINE__);\
        printf("\t%s\n", str);\
        return 1;\
    }while(0)

#define DBG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define EVTDEF(evt) {evt, #evt}
struct _evt_str {
    int evt;
    char *str;
} str_evts [] = {
    EVTDEF(EPOLLIN),
    EVTDEF(EPOLLOUT),
    EVTDEF(EPOLLRDHUP),
    EVTDEF(EPOLLPRI),
    EVTDEF(EPOLLERR),
    EVTDEF(EPOLLHUP),
    EVTDEF(EPOLLET),
    EVTDEF(EPOLLONESHOT),
    EVTDEF(EPOLLWAKEUP),
    /*EVTDEF(EPOLLEXCLUSIVE),*/
};
int sz_str = sizeof(str_evts) / sizeof(str_evts[0]);
void put_evts(uint32_t evts) {
    int i;

    DBG("Dump events:");
    for(i = 0; i < sz_str; i++) {
        if(evts & str_evts[i].evt)
            DBG("\t%s", str_evts[i].str);
    }
}
int main(void) {
    int i;
    int fin, fout;
#if 0
    fin = open(STDIN_FILENO, O_RDONLY);
    if(fin == -1)
        FAIL("fin open");

    fout = open(STDOUT_FILENO, O_WRONLY);
    if(fout == -1)
        FAIL("fout open");
#else
    fin = STDIN_FILENO;
    fout = STDOUT_FILENO;
#endif

    DBG("fd: fin %d fout %d", fin, fout);

    int epfd = epoll_create1(0);// or EPOLL_CLOEXEC
    if(epfd == -1)
        FAIL("epfd create");

    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fin;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, fin, &evt) == -1)
        FAIL("fin add");

    evt.events = EPOLLOUT;
    evt.data.fd = fout;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, fout, &evt) == -1)
        FAIL("fout add");

    struct epoll_event *pevt = (struct epoll_event *)malloc(sizeof(struct epoll_event) * 2);
    if(!pevt)
        FAIL("Malloc evts");

    DBG("Now wait for fin and fout, timeout 5s, fout shall be ok");

    int ret = epoll_wait(epfd, pevt, 2, 5 * 1000);// wait 5s at most
    if(ret == -1)
        FAIL("Wait 2 evts");
    else if(ret == 0)
        DBG("fin and fout timeout");
    else {
        for(i = 0; i < ret; i++){
            DBG("%d: evts %X fd %d", i, pevt[i].events, pevt[i].data.fd);
            put_evts(pevt[i].events);
        }
    }

    DBG("Now wait for fin only, please do not type to test wait time out");
    if(epoll_ctl(epfd, EPOLL_CTL_DEL, fout, NULL) == -1)
        FAIL("Del fout");

    ret = epoll_wait(epfd, pevt, 2, 5 * 1000);// wait 5s at most
    if(ret == -1)
        FAIL("Wait 2 evts");
    else if(ret == 0)
        DBG("fin and fout timeout");
    else {
        if(ret > 1)
            FAIL("Too much evts");

        for(i = 0; i < ret; i++){
            DBG("%d: evts %X fd %d", i, pevt[i].events, pevt[i].data.fd);
            put_evts(pevt[i].events);
        }
    }


    DBG("Now wait for fin only, please type to test input trigger event");
    DBG("Note that Enter is required to trigger input event");

    ret = epoll_wait(epfd, pevt, 2, 5 * 1000);// wait 5s at most
    if(ret == -1)
        FAIL("Wait 2 evts");
    else if(ret == 0)
        DBG("fin and fout timeout");
    else {
        if(ret > 1)
            FAIL("Too much evts");

        for(i = 0; i < ret; i++){
            DBG("%d: evts %X fd %d", i, pevt[i].events, pevt[i].data.fd);
            put_evts(pevt[i].events);
        }
    }

    if(close(epfd))
        FAIL("epfd close");

    free(pevt);

    if(close(fin))
        FAIL("fin close");

    if(close(fout))
        FAIL("fout close");

    return 0;
}
