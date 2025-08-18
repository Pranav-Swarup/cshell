#include <stdio.h>
#include "activities_util.h"

int cmd_activities(const Atomic *atomic) {
    printf("[activities] called with %d arguments:\n", atomic->argcount);

    return 0;
}
