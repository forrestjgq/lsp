
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>

#define DBG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define FAIL(str)\
    do { \
        if(errno)\
            perror("errno");\
        printf("fail @ %s:%d\n", __FILE__, __LINE__);\
        printf("\t%s\n", str);\
        return 1;\
    }while(0)

int main(int argc, char *argv[]) {
    if(argc < 2) {
        DBG("Usage: %s <file>", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1)
        FAIL("Open");

    struct stat st;
    if(fstat(fd, &st) == -1)
        FAIL("fstat");

    if(!S_ISREG(st.st_mode))
        FAIL("Not Regular File");

    char *addr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(MAP_FAILED == addr)
        FAIL("mmap");

    if(close(fd))
        FAIL("Close");

    int i;
    for(i = 0; i < st.st_size; i++)
        putchar(addr[i]);

    if(munmap(addr, st.st_size) == -1)
        FAIL("munmap");

    return 0;
}
