#ifndef PARSER_H
#define PARSER_H
#include <stddef.h>

typedef enum {
    CONN_NONE,   // END OF CMD line
    CONN_SEQ,    // ;
    CONN_ANDAND,    // &&
    CONN_AND      // &
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
    int background;		// Default set to 0. When background TRUE, parser.c sets to 1
} CommandBlock;

typedef struct {
    CommandBlock blocks[16];
    int blockcount;
} ParsedLine;

ParsedLine parse_line(const char *line);

#endif
