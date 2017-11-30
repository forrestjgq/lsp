#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BUFLEN  200
int buf[BUFLEN];
int rdbuf[BUFLEN*2];
#define DBG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define TEST_FILE  "./rw.test"

int main(void) {
    int fd;
    ssize_t sz;
    size_t len;

    len = sizeof(buf);
    char *p = (char *)buf;

    for(sz = 0; sz < BUFLEN; sz++)
        buf[sz] = sz * 2;

    fd = open(TEST_FILE, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if(fd == -1) {
        perror("create");
        return 1;
    }


    while(len != 0 && (sz = write(fd, p, len)) != 0) {
        if(sz == -1) {
            if(errno == EINTR)
                continue;
            perror("write");
            close(fd);
            return 3;
        }

        len -= sz;
        p += sz;
    }

    close(fd);
    DBG("Write OK");

    fd = open(TEST_FILE, O_RDONLY);
    if(fd == -1) {
        perror("read open");
        return 2;
    }

    len = sizeof(rdbuf);
    p = (char *)rdbuf;
    while(len > 0 && (sz = read(fd, p, len)) != 0) {
        if(sz == -1) {
            if(errno == EINTR){
                continue;
            }
            perror("read");
            close(fd);
            return 4;
        }

        len -= sz;
        p += sz;
    }
    close(fd);

    DBG("Read OK");
    if(p - (char *)rdbuf != sizeof(buf) ||
       0 != memcmp(rdbuf, buf, sizeof(buf))){
        fprintf(stderr, "read write content mismatch\n");
        return 5;
    }

    fd = open(TEST_FILE, O_WRONLY);
    if(fd == -1) {
        perror("last write");
        return 6;
    }

    off_t pos = lseek(fd, 0, SEEK_END);
    if(pos == (off_t) -1) {
        perror("lseek");
        close(fd);
        return 7;
    }
    DBG("curr file size %d", pos);

    /**
     * Seek beyond ending, padding with 0 after write
     */
    off_t pos1 = lseek(fd, 1080, SEEK_END);
    if(pos1 == (off_t) -1) {
        perror("lseek1");
        close(fd);
        return 7;
    }

    DBG("2: curr file size %d", pos1);

    len = sizeof(buf);
    p = (char *)buf;

    while(len != 0 && (sz = write(fd, p, len)) != 0) {
        if(sz == -1) {
            if(errno == EINTR)
                continue;
            perror("write");
            close(fd);
            return 3;
        }

        len -= sz;
        p += sz;
    }

    /**
     * See beyond start
     */
    pos1 = lseek(fd, -20, SEEK_SET);
    if(pos1 == (off_t)-1) {
        perror("seek before 0");
    }

    pos1 = lseek(fd, 0, SEEK_CUR);
    if(pos1 == (off_t)-1) {
        perror("seek for trunc");
        close(fd);
        return 9;
    }

    if(-1 == ftruncate(fd, pos1/2)) {
        perror("ftrunc");
        close(fd);
        return 10;
    }
    pos = lseek(fd, 0, SEEK_CUR);
    if(pos == (off_t)-1) {
        perror("seek for trunc");
        close(fd);
        return 9;
    }

    /**
     * trancate won't change the position
     */
    DBG("pos: before %d after %d", pos1, pos);

    /**
     * truncate to a larger file, padding with 0
     */
    if( -1 == ftruncate(fd, pos * 2)) {
        perror("trunc large");
        close(fd);
        return 9;
    }

    close(fd);

    DBG("DONE");
    return 0;


}
