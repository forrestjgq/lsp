#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define DBG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define CH_CNT 3
#define STR_BUF_LEN 64

/**
 * Get #(35, 0x23)
 * Get i(105, 0x69)
 * Get n(110, 0x6E)
 * Now unget them
 * Now re-get them
 * Get n(110, 0x6E)
 * Get i(105, 0x69)
 * Get #(35, 0x23)
 * last ch 0x0a of clude <stdio.h>
 */
int main(void) {
    FILE *f ;//= (FILE *)STDIN_FILENO;
    f = fopen("io.c", "r");
    if(!f) {
        perror("open test");
        return 1;
    }

    if(setvbuf(f, NULL, _IOFBF, 4*1024) != 0)
        DBG("setvbuf err %d", ferror(f));

    DBG("BUFSIZE: %d", BUFSIZ);

    int cnt;
    int c;
    char chars[CH_CNT];
    for(cnt = 0; cnt < CH_CNT; cnt++) {
        c = fgetc(f);
        if(c == EOF) {
            DBG("EOF in fgetc");
            return 2;
        }

        DBG("Get %c(%d, 0x%02X)", (char)c, c, c);
        chars[cnt] = c;
    }

    DBG("Now unget them");

    for(cnt = 0; cnt < CH_CNT; cnt++) {
        c = ungetc((int)chars[cnt], f);
        if(c == EOF) {
            DBG("EOF in ungetc");
            return 2;
        }
    }

    DBG("Now re-get them");
    for(cnt = 0; cnt < CH_CNT; cnt++) {
        c = fgetc(f);
        if(c == EOF) {
            DBG("EOF in re-fgetc");
            return 2;
        }

        DBG("Get %c(%d, 0x%02X)", (char)c, c, c);
        if(chars[CH_CNT - cnt - 1] != (char) c) {
            DBG("Mismatch between fgetc and ungetc");
        }
    }

    char str[STR_BUF_LEN];
    if(NULL == fgets(str, STR_BUF_LEN, f)){
        perror("fgets");
        return 5;
    }
    DBG("last ch 0x%02x of %s", str[strlen(str)-1], str);

    fclose(f);

    int nr = 4;
    int sz = 8;

    f = fopen("test", "w+");
    if(!f) {
        perror("open test");
        return 1;
    }

    int wr = nr * sz - 1;
    char *buf = (char *)malloc(wr+1);
    if(!buf) {
        DBG("No mem");
        return 9;
    }
    memset(buf, 0, nr * sz);

    if(wr != fwrite(buf, 1, wr, f)) {
        perror("bin write");
        free(buf);
        return 9;
    }

    /**
     * flush NULL indicates flush all files of current process
     */
    if(EOF == fflush(NULL)) {
        perror("fflush");
        return 9;
    }

    if( -1 == fseek(f, 0, SEEK_SET)) {
        perror("seek");
        free(buf);
        return 9;
    }

    int rd = fread(buf, sz, nr, f);
    DBG("Wr %dB Rd %dB", wr,rd);
    if(rd < nr) {
        if(feof(f))
            /**
             * In this test, here it runs
             */
            DBG("Rd EOF");
        else {
            int err = ferror(f);
            if(err){
                DBG("Rd err %d", err);
            }
        }

        clearerr(f);

        long pos = ftell(f);
        if(pos == -1) {
            perror("ftell");
            return 9;
        }
        DBG("Pos: %d", pos);

        errno = 0;//clear errno
        rewind(f);
        if(errno){
            perror("rewind");
            return 9;
        }

        pos = ftell(f);
        if(pos == -1) {
            perror("ftell after rewind");
            return 9;
        }
        DBG("Pos after rewind: %d", pos);

    }

    free(buf);
    fclose(f);

}
