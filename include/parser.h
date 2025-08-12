#ifndef PARSER_H
#define PARSER_H

typedef enum{
    TOK_NAME,       // cmd or arg
    TOK_PIPE,       // |
    TOK_BG,         // &
    TOK_AND,        // &&
    TOK_RED_IN,     // <
    TOK_RED_OUT,    // >
    TOK_RED_APPEND, // >>
    TOK_SEMICOLON,  // ;
    TOK_EOF         // End of input
} TokenType;

typedef enum {
    NODE_SHELL_CMD,
    NODE_CMD_GROUP,
    NODE_ATOMIC,
    NODE_SEQUENTIAL
} NodeType;

typedef struct Node {
    NodeType type;
    union {
        struct {
            struct Node *cmd_group;
            char *connector;
            struct Node *next;
            int background;
        } shell_cmd;
        struct {
            struct Node *atomic;
            struct Node *next;
        } cmd_group;
        struct {
            char *command;
            char **args;
            int arg_count;
            char *input;
            char *output;
            int append;
        } atomic;
        struct {
            struct Node *first;
            struct Node *second;
        } sequential;
    } data;
} Node;

Node *parse(char *input);
void free_node(Node *node);
void print_node(Node *node, int indent);

#endif
