#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>


#define DBG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define FAIL(str)\
    do { \
        if(errno)\
            perror("errno");\
        printf("fail @ %s:%d\n", __FILE__, __LINE__);\
        printf("\t%s\n", str);\
        return 1;\
    }while(0)

#define PERR(fmt, ...) dprintf(STDOUT_FILENO, fmt "\n", ##__VA_ARGS__)
/**
 * This example demostrate user id and group id
 */
int main(int argc, char *argv[]) {
    int fout = STDOUT_FILENO;
    int fd = STDIN_FILENO;
    int ferr = STDERR_FILENO;

    PERR("Program: %s", argv[0]);

    dprintf(ferr, "====> pid %jd ppid %jd\n", (intmax_t)getpid(), (intmax_t)getppid());
    dprintf(ferr, "====> uid %jd euid %jd\n", (intmax_t)getuid(), (intmax_t)geteuid());
    dprintf(ferr, "\n");

    char c;
    int exit = 0;
    while(!exit && read(fd, &c, 1) != -1){
        /*DBG("GET %c", c);*/
        if(c >= 0 && c <= 9)
            c += '0';
        else if(c >= '0' && c <= '8')
            c ++;
        else if(c == '9')
            c = '0';
        else if(c == 'q')
            exit = 1;
        else
            continue;

        write(fout, &c, 1);
    }

    return 0;
}
