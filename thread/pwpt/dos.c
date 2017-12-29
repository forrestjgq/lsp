#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char old[128];
char buf[1024 * 1024 * 10];
int main(int argc, char *argv[])
{
    if(argc != 2) {
        printf("no file given\n");
        return 1;
    }

    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        printf("open %s fail\n", argv[1]);
        return 2;
    }

    strcpy(old, argv[1]);
    strcat(old, ".old");
    int fd1 = open(old, O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if(fd1 == -1) {
        printf("open bkup file %s fail\n", old);
        return 3;
    }

    int cnt = read(fd, buf, sizeof(buf));
    if(cnt <= 0) {
        printf("read file fail\n");
        return 4;
    }
    printf("file size %d bytes\n", cnt);
    close(fd);

    int wrcnt = write(fd1, buf, cnt);
    if(wrcnt != cnt) {
        printf("write bkup fail\n");
        return 5;
    }
    close(fd1);
    fd = open(argv[1], O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if(fd1 == -1) {
        printf("open write file %s fail\n", argv[1]);
        return 3;
    }

    lseek(fd, 0, SEEK_SET);

    int i;
    for(i = 0; i < cnt; i++) {
        if(buf[i] == 0x0d) {
            if(buf[i+1] == 0x0a)
                continue;
            buf[i] = 0x0a;
        }

        if(write(fd, &buf[i], 1) != 1) {
            printf("write changed file fail, you may recover from %s", old);
            return 6;
        }
    }

    close(fd);

    return 0;


}
