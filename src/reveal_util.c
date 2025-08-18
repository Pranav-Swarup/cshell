#include <stdio.h>
#include "reveal_util.h"

int cmd_reveal(const Atomic *atomic) {
    printf("[reveal] called with %d arguments:\n", atomic->argcount);

    for (int i = 0; i < atomic->argcount; i++) {
        printf("  argv[%d] = %s\n", i, atomic->argv[i]);
    }

    if (atomic->argcount == 1) {
        printf("[reveal] No argument -> would reveal HOME dir\n");
    } else {
        printf("[reveal] Would show files of: %s\n", atomic->argv[1]);
    }

    return 0;
}
