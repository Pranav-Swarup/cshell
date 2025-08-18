#include <stdio.h>
#include "log_util.h"

int cmd_log(const Atomic *atomic) {
    printf("[log] called with %d arguments:\n", atomic->argcount);

    return 0;
}
