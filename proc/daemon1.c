#include <stdio.h>
#include <unistd.h>
#include "dbg.h"

/**
 * This test damonstrate daemon()
 */
int main(int argc, char *argv[]) {

    if(daemon(0, 0) == -1)
        FAIL("daemon");

    while(1) ;

    return 0;

}
