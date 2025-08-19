#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include "prompt.h"
#include "hop_util.h"


static int comparestr(const void *a, const void *b) {
    char *const *sa = (char *const *)a;
    char *const *sb = (char *const *)b;
    return strcmp(*sa, *sb);
}

int cmd_reveal(const Atomic *atomic) {
    int showall = 0;
    int lines = 0;
    const char *target = ".";

    int i = 1;
    for(i = 1; i < atomic->argcount; i++){
        if(atomic->argv[i][0] != '-')
        	break;
        for(int j = 1; atomic->argv[i][j] != '\0'; j++){
            if(atomic->argv[i][j] == 'a')
            	showall = 1;
            else if(atomic->argv[i][j] == 'l')
            	lines = 1;
        }
    }

    if(i < atomic->argcount){
        char *arg = atomic->argv[i];
        if(strcmp(arg, "~") == 0){
            target = home ? home : getenv("HOME");
        }
        else{
            target = arg;
        }
    }

    DIR *dir = opendir(target);
    if(!dir){
        printf("No such directory!\n");
        return 1;
    }

    struct dirent *entry;
    char **names = NULL;
    size_t count = 0;

    while((entry = readdir(dir)) != NULL){
        if(!showall && entry->d_name[0] == '.')
        	continue;
        names = realloc(names, (count + 1) * sizeof(char *));
        names[count] = strdup(entry->d_name);
        count++;
    }
    closedir(dir);

    qsort(names, count, sizeof(char *), comparestr);

    for (size_t k = 0; k < count; k++) {
        if(lines)
        	printf("%s\n", names[k]);
        else{
            printf("%s", names[k]);
            if(k != count - 1)
            	printf(" ");
            else
            	printf("\n");
        }
        free(names[k]);
    }
    free(names);

    return 0;
}
