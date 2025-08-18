#include <stdio.h>
#include "hop_util.h"

int cmd_hop(const Atomic *atomic) {
    printf("[hop] called with %d arguments:\n", atomic->argcount);

    for (int i = 0; i < atomic->argcount; i++) {
        printf("  argv[%d] = %s\n", i, atomic->argv[i]);
    }

    if(atomic->argcount == 1){
        printf("[hop] No argument -> would go to HOME dir\n");
    }
    else{
        printf("[hop] Would chdir to: %s\n", atomic->argv[1]);
    }

    return 0;
}
