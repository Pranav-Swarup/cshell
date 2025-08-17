#ifndef CMD_ARCHIVE_H
#define CMD_ARCHIVE_H

#include <stdbool.h>

typedef struct{
    const char *name;
    bool dont_fork;
    bool allow_input_redir;
    bool allow_output_redir;
    bool allow_pipe;
    const char *help;
} CommandInfo;

const CommandInfo *get_cmd_info(const char *s);
extern const CommandInfo cmd_table[];

// Useless
extern const int cmd_table_size;

#endif 
