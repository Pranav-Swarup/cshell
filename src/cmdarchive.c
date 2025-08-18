#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cmdarchive.h"

const CommandInfo cmd_table[] = {
    { "hop",        true,  false, false, false, "Change current working directory" },
    { "reveal",     false, false, true,  true,  "Reveal file contents or details" },
    { "log",        false, false, true,  true,  "Display shell activity log" },
    { "activities", false, false, true,  true,  "List background jobs" },
    { "ping",       false, false, true,  true,  "Send ping to host" },
    { "fg",         true,  false, false, false, "Bring background job to foreground" },
    { "bg",         true,  false, false, false, "Resume stopped job in background" },
};

const int cmd_table_size = sizeof(cmd_table) / sizeof(cmd_table[0]);


const CommandInfo *get_cmd_info(const char *cmd_name){
    for(int i = 0; i < cmd_table_size; i++){
        if(strcmp(cmd_name, cmd_table[i].name) == 0){
            return &cmd_table[i];
        }
    }
    return NULL;
}
