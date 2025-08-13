#ifndef PARSER_H
#define PARSER_H
#include <stddef.h>

typedef enum {
    CONN_NONE,   // END OF CMD line
    CONN_SEQ,    // ;
    CONN_AND,    // &&
    CONN_BG      // &
} ConnectorType;

typedef struct {
    char *argv[16];
    int argcount;
    char *input;          // < file
    char *output;         // > file
    int append;           // 1 if >>, 0 if >
} Atomic;

typedef struct {
    Atomic atomics[16];
    int atomic_count;
    ConnectorType connector;
} CommandBlock;

typedef struct {
    CommandBlock blocks[16];
    int blockcount;
    int background; // 1 if final background '&' is set
} ParsedLine;

ParsedLine parse_line(const char *line);

void print_parsed(const ParsedLine *pl);

#endif
