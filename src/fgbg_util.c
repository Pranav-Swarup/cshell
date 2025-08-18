#include <stdio.h>
#include "fgbg_util.h"

int cmd_fg(const Atomic *atomic) {
    printf("[fg] called with %d arguments:\n", atomic->argcount);

    return 0;
}


int cmd_bg(const Atomic *atomic) {
    printf("[bg] called with %d arguments:\n", atomic->argcount);

    return 0;
}
