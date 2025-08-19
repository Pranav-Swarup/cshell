#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "hop_util.h"
#include "prompt.h"

static char prev_dir[PATH_MAX] = {0};

int cmd_hop(const Atomic *atomic){
    char cwd[PATH_MAX];

    if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("error in getcwd");
        return -1;
    }

    if(atomic->argcount == 1){
        if(chdir(home) != 0){
            perror("hop to home error");
            return -1;
        }
        strncpy(prev_dir, cwd, sizeof(prev_dir));
        return 0;
    }

    for(int i = 1; i < atomic->argcount; i++){
        const char *arg = atomic->argv[i];

        if(strcmp(arg, "~") == 0){
            if(chdir(home) != 0){
                perror("hop to home error");
                return -1;
            }
            strncpy(prev_dir, cwd, sizeof(prev_dir));
        }
        else if(strcmp(arg, ".") == 0){
            continue;
        }
        else if(strcmp(arg, "..") == 0){
            if(chdir("..") != 0){
                perror("hop to parent dir");
                return -1;
            }
            strncpy(prev_dir, cwd, sizeof(prev_dir));
        }
        else if(strcmp(arg, "-") == 0){
            if(strlen(prev_dir) == 0)
            	continue;
            if(chdir(prev_dir) != 0){
                printf("No such directory! - prev dir\n");
                return -1;
            }
            strncpy(prev_dir, cwd, sizeof(prev_dir));
        }
        else{
            if(chdir(arg) != 0){
                printf("No such directory!\n");
                return -1;
            }
            strncpy(prev_dir, cwd, sizeof(prev_dir));
        }

        if(getcwd(cwd, sizeof(cwd)) == NULL){
            perror("error in getcwd");
            return -1;
        }
    }

    return 0;
}
