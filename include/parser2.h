// parser2.h
#ifndef PARSER2_H
#define PARSER2_H

#include <stddef.h>

#define MAX_ARGS     16
#define MAX_ATOMICS  16
#define MAX_BLOCKS   16

// Connector types between command blocks
typedef enum {
    CONN_NONE,   // No connector (end of command line)
    CONN_SEQ,    // ;
    CONN_AND,    // &&
    CONN_BG      // &
} ConnectorType;

// A single command with its arguments and optional redirections
typedef struct {
    char *argv[MAX_ARGS]; // Command + args, NULL terminated in real execution
    int argc;             // Number of arguments
    char *input;          // < file
    char *output;         // > file
    int append;           // 1 if >>, 0 if >
} Atomic;

// A pipeline of atomics connected by pipes
typedef struct {
    Atomic atomics[MAX_ATOMICS];
    int atomic_count;
    ConnectorType connector; // Connector to the next block
} CommandBlock;

// The full parsed line: sequence of blocks
typedef struct {
    CommandBlock blocks[MAX_BLOCKS];
    int block_count;
    int background; // 1 if final background '&' was set
} ParsedLine;

// Parse a command line into ParsedLine structure
ParsedLine parse_line(const char *line);

// Print the parsed structure (for debugging)
void print_parsed(const ParsedLine *pl);

#endif // PARSER2_H
