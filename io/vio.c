
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>


#define DBG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define VCNT 5
#define TFILE "test"

struct iovec rd[VCNT], wr[VCNT];
int main(void) {

    int fd = open(TFILE, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(fd == -1) {
        perror("open write");
        return 1;
    }

    unsigned char c = 0;
    int length = 0;

    int i, j, k;
    for(i = 0; i < VCNT; i++) {
        int j = VCNT - i - 1;

        rd[i].iov_len = wr[j].iov_len = 1024 * (1 << i) + i;
        length += rd[i].iov_len;

        rd[i].iov_base = malloc(rd[i].iov_len);
        wr[j].iov_base = malloc(wr[j].iov_len);

        if(!rd[i].iov_base || !wr[j].iov_base){
            DBG("No mem");
            return 2;
        }
    }

    /**
     * init vector data to write
     */
    for(i = 0; i < VCNT; i++) {
        unsigned char * p = (unsigned char *)wr[i].iov_base;
        for(j = 0; j < wr[i].iov_len; j++) {
            p[j] = c++;
        }
    }

    int ret = writev(fd, wr, VCNT);
    if(ret == -1) {
        perror("writev");
        return 2;
    }

    if(ret != length)
        DBG("Write %d less than expected %d", ret, length);

    if( close(fd) ) {
        perror("close wr");
        return 1;
    }


    fd = open(TFILE, O_RDONLY);
    if(fd == -1) {
        perror("open read");
        return 1;
    }
    for(i = 0; i < VCNT; i++)
        DBG("RDV[%d] len %d", i, rd[i].iov_len);

    errno = 0;
    ret = readv(fd, rd, VCNT);
    if(ret == -1) {
        perror("readv");
        return 2;
    }

    if(ret != length) {
        perror("ret");
        DBG("Read %d less than expected %d", ret, length);
    }

    if( close(fd) ) {
        perror("close rd");
        return 1;
    }

    for(i = 0; i < VCNT; i++) {
        free(rd[i].iov_base);
        free(wr[i].iov_base);
    }
    return 0;
}
