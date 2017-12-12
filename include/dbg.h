#ifndef __DBG__H__
#define __DBG__H__

#include <stdio.h>
#include <errno.h>
#include <assert.h>

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

#define PERR(fmt, ...) dprintf(STDERR_FILENO, fmt "\n", ##__VA_ARGS__)

#define ENSURE(expr) \
    do {\
        if(!(expr)) {\
            DBG("Fail @ file %s:%d", __FILE__, __LINE__);\
            assert(expr);\
        }\
    }while(0)
extern const char *strsig(int sig);
#endif /* __DBG__H__ */
