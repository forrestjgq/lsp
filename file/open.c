#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>
#include <stdlib.h>

#define FAIL(str)\
    do { \
        perror("errno");\
        printf("fail @ %s:%d\n", __FILE__, __LINE__);\
        printf("\t%s\n", str);\
        return 1;\
    }while(0)

int main(void) {
   int fd;

   ssize_t len;
   const char * str = "this is a test";
   int tlen = strlen(str);
   /**
    * open read only with write only
    */
   fd = open("/etc/passwd", O_WRONLY);
   if(fd == -1)
       perror("open /etc/passwd with write only");
   else
       close(fd);


   /**
    * open read only with read and write
    */
   fd = open("/etc/passwd", O_RDWR);
   if(fd == -1)
       perror("open /etc/passwd with read and write");
   else
       close(fd);

   /**
    * open read only file
    */
   fd = open("/etc/passwd", O_RDONLY);
   if(fd == -1)
       perror("open /etc/passwd fail");
   else
       close(fd);

   /**
    * create a new file
    * mode must be given, here is 0666
    * if not, the permission is undefined(unknown)
    */
   fd = open("./open.test", O_RDWR | O_CREAT, 0666);
   if(fd == -1)
       FAIL("create file fail");
   else
       write(fd, str, tlen);
   close(fd);

    /**
     * open exist file with O_CREAT
     */
   fd = open("./open.test", O_RDWR | O_CREAT, 0666);
   if(fd == -1)
       FAIL("open fail");
   else {
       char *buf = (char *)malloc(tlen);
       if(!buf)
           FAIL("No mem");
       else {
            len = read(fd, buf, tlen);
            if(len != tlen)
                FAIL("Read fail len");
            else if(0 != memcmp(str, buf, tlen))
                FAIL("Invalid content");
       }
       free(buf);
   }
   close(fd);

    /**
     * open exist file with O_CREAT and O_EXCL
     */
   fd = open("./open.test", O_RDWR | O_CREAT | O_EXCL, 0666);
   if(fd != -1)
       FAIL("open should fail");
   close(fd);

    /**
     * open exist file with O_TRUNC
     */
   fd = open("./open.test", O_RDWR | O_TRUNC);
   if(fd == -1)
       FAIL("open fail");
   else {
       char *buf = (char *)malloc(tlen);
       if(!buf)
           FAIL("No mem");
       else {
            len = read(fd, buf, tlen);
            if(len != 0)
                FAIL("Read fail len");
       }
       free(buf);
   }
   close(fd);

   printf("succeed\n");
   return 0;
}
