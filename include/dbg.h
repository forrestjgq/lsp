#ifndef __DBG__H__
#define __DBG__H__

#include <stdio.h>
#include <errno.h>

#define DBG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define SEPERATE() DBG("=================================================")

#define FAIL(str)\
    do { \
        if(errno)\
            perror("errno");\
        printf("fail @ %s:%d\n", __FILE__, __LINE__);\
        printf("\t%s\n", str);\
        return 1;\
    }while(0)

#define PERR(fmt, ...) dprintf(STDOUT_FILENO, fmt "\n", ##__VA_ARGS__)
#endif /* __DBG__H__ */
