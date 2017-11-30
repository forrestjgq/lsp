
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <unistd.h>
/*#include <stdlib.h>*/
#include <errno.h>

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

    int i;
    for(i = 1; i < argc; i++){
        DBG("Try %s", argv[i]);
        DBG("==============");

        int fd = open(argv[1], O_RDONLY);
        if(fd == -1) {
            perror("open");
            continue;
        }

        struct stat st;
        if(fstat(fd, &st) == -1) {
            perror("fstat");
            goto CLEANUP;
        }

        DBG("inode nr: %d", st.st_blocks);

        ino_t nb = st.st_blocks;
        int j;
        for(j = 0; j < nb; j++) {
            int logic_block = j;
            errno = 0;
            if(ioctl(fd, FIBMAP, &logic_block) == -1 && errno) {
                /**
                 * super user is required to perform ioctl on FBIMAP
                 */
                perror("ioctl");
                break;
            }
            DBG("Block %d: %d", j, logic_block);
        }
    CLEANUP:
        if(close(fd) == -1)
            perror("close");

    }

    return 0;
}
