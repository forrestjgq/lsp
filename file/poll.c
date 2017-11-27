#include <stdio.h>

#include <unistd.h>
#include <poll.h>

#define TIMEOUT  5

#define DBG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
/**
 * execute:
 *
 * forrest@ubuntu:~/lsp/file$ gcc poll.c;./a.out  < rw.c
 * Poll ret 2
 * Read ready
 * Write ready
 * forrest@ubuntu:~/lsp/file$ ./a.out
 * Poll ret 1
 * Write ready
 */
int main(void) {
    struct pollfd fds[2];

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    fds[1].fd = STDOUT_FILENO;
    fds[1].events = POLLOUT;

    int ret = poll(fds, 2, TIMEOUT * 1000);
    DBG("Poll ret %d", ret);
    if(ret == -1) {
        perror("poll");
        return 1;
    }

    if(!ret) {
        DBG("Timeout");
        return 2;
    }

    if(fds[0].revents & POLLIN)
        DBG("Read ready");

    if(fds[1].revents & POLLOUT)
        DBG("Write ready");

    return 0;
}
