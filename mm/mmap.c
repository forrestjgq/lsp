/**
 * This macro must be defined before any real code
 * due to <features.h> MAY be included by any files
 * included before mman.h, and that will make feature.h
 * be included before macro definition
 *
 * The best way to implement is to define it by gcc
 * command line arguments
 */
#define _GNU_SOURCE
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

/**
 * If we define _GNU_SOURCE here instead of in the begging
 * of this file, it won't work
 */
/*#define _GNU_SOURCE*/
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
    int i;
    int fd;
    char *pwr, *addr;
    struct stat st;
#define PUT_MEM()\
    do{\
        for(i = 0; i < st.st_size; i++)\
            if(addr[i])\
                putchar(addr[i]);\
            else {\
                putchar('\\');\
                putchar('0');\
            }\
    }while(0)

    if(argc < 2) {
        DBG("Usage: %s <file>", argv[0]);
        return 1;
    }

    DBG("Open file %s", argv[1]);

    /**
     * Here O_RDWR is a must.
     * if O_RDONLY is used, mprotect will fail to change
     * the permission to WRITE
     */
    fd = open(argv[1], /*O_RDONLY*/O_RDWR);


    if(fd == -1)
        FAIL("Open");

    if(fstat(fd, &st) == -1)
        FAIL("fstat");

    if(!S_ISREG(st.st_mode))
        FAIL("Not Regular File");

    addr = mmap(NULL, st.st_size/2, PROT_READ, MAP_SHARED, fd, 0);
    if(MAP_FAILED == addr)
        FAIL("mmap");
    DBG("Start map size %d addr %p", st.st_size/2, addr);

    DBG("Advise");
    if(madvise(addr, st.st_size/2, MADV_SEQUENTIAL) == -1)
        FAIL("madvise");

    DBG("Close FD");
    if(close(fd))
        FAIL("Close");

    DBG("Print to size %d", st.st_size);
    PUT_MEM();

    addr = mremap(addr, st.st_size/2, st.st_size, MREMAP_MAYMOVE);
    if(addr == MAP_FAILED)
        FAIL("Remap");
    DBG("Remap to size %d addr %p", st.st_size, addr);


    DBG("Advise2");
    if(madvise(addr, st.st_size, MADV_SEQUENTIAL) == -1)
        FAIL("madvise");

    DBG("Print to size %d", st.st_size);
    PUT_MEM();

    /**
     * mprotect only accept an addr which is page aligned
     */
    pwr = addr + getpagesize();

    if(mprotect(pwr, 10, PROT_WRITE) == -1)
        FAIL("mprotect");

    DBG("Print after change protect to write");
    for(i = 0; i < 10; i++)
        pwr[i] = i + '0';

    /**
     * Although mprotect change permission to WRITE
     * It is still readable for i386
     * which means PROT_WRITE implies PROT_READ
     */
    PUT_MEM();

    if(msync(addr, st.st_size, MS_SYNC) == -1)
        FAIL("msync");

    if(munmap(addr, st.st_size) == -1)
        FAIL("munmap");

    /**
     * The following phrase will trigger a segmentation fault
     */
    /*pwr[0] = 'a';*/

    return 0;
}
