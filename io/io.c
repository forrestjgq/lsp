#include <stdio.h>
#include <string.h>
#include <unistd.h>

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


}
