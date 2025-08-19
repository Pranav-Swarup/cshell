#include <pwd.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "prompt.h"

//############## LLM Generated Code Begins ##############

#define _POSIX_C_SOURCE 200809L

//############## LLM Generated Code Ends ##############

static char *username = NULL;
static char *hostname = NULL;
char *home = NULL;					// externed in prompt.h file

void get_prompt_info(){
    struct passwd *pw = getpwuid(getuid());
    if(pw && pw->pw_name)
    	username = strdup(pw->pw_name);
    else
    	username = strdup("user");

    char hostbuf[256];
    if(gethostname(hostbuf, sizeof(hostbuf)) == 0)
    	hostname = strdup(hostbuf);
    else
    	hostname = strdup("host");

    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL)
    	home = strdup(cwd);
    else
    	home = strdup("/");
}

char *display_path(void){

    static char buf[PATH_MAX];
    char cwd[PATH_MAX];

    if(getcwd(cwd, sizeof(cwd)) == NULL){
        strncpy(buf, "NULL", sizeof(buf));
        buf[sizeof(buf)-1] = '\0';
        return buf;
    }

    int hlen = strlen(home);
    
    if(hlen > 1 && strncmp(cwd, home, hlen) == 0 && (cwd[hlen] == '/' || cwd[hlen] == '\0')){
        if(cwd[hlen] == '\0')
        	snprintf(buf, sizeof(buf), "~");
        else
        	snprintf(buf, sizeof(buf), "~%s", cwd + hlen);
    }
    else
        snprintf(buf, sizeof(buf), "%s", cwd);

    return buf;
}

void prompt_printer() {
    char *path = display_path();
    printf("<%s@%s:%s> ", username, hostname, path);
    fflush(stdout);
}

void prompt_cleaner() {
    free(username); username = NULL;
    free(hostname); hostname = NULL;
    free(home); home = NULL;
}
