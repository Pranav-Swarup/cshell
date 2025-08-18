#include <stdio.h>
#include "ping_util.h"

int cmd_ping(const Atomic *atomic) {
    printf("[ping] called with %d arguments:\n", atomic->argcount);

    return 0;
}
