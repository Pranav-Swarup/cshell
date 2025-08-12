// parser.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

// Token structure (not exposed in parser.h)
typedef struct {
    TokenType type;
    char *value;
} Token;

// Tokenizer structure (not exposed in parser.h)
typedef struct {
    char *input;
    int pos;
    Token *current_token;
} Tokenizer;

// Parser structure (not exposed in parser.h)
typedef struct {
    Tokenizer *tokenizer;
} Parser;

// Implementation of tokenizer functions
Token *new_token(TokenType type, char *value) {
    Token *token = malloc(sizeof(Token));
    token->type = type;
    token->value = value ? strdup(value) : NULL;
    return token;
}

void free_token(Token *token) {
    if (token->value) free(token->value);
    free(token);
}

Tokenizer *init_tokenizer(char *input) {
    Tokenizer *t = malloc(sizeof(Tokenizer));
    t->input = input;
    t->pos = 0;
    t->current_token = NULL;
    return t;
}

void skip_whitespace(Tokenizer *t) {
    while (t->input[t->pos] && isspace(t->input[t->pos])) {
        t->pos++;
    }
}

Token *get_next_token(Tokenizer *t) {
    skip_whitespace(t);
    if (!t->input[t->pos]) {
        return new_token(TOK_EOF, NULL);
    }
    char c = t->input[t->pos];
    char *start = t->input + t->pos;
    int len = 0;
    if (c == '|') {
        t->pos++;
        return new_token(TOK_PIPE, NULL);
    } else if (c == '&') {
        t->pos++;
        if (t->input[t->pos] == '&') {
            t->pos++;
            return new_token(TOK_AND, "&&");
        }
        return new_token(TOK_BG, "&");
    } else if (c == '<') {
        t->pos++;
        return new_token(TOK_RED_IN, "<");
    } else if (c == '>') {
        t->pos++;
        if (t->input[t->pos] == '>') {
            t->pos++;
            return new_token(TOK_RED_APPEND, ">>");
        }
        return new_token(TOK_RED_OUT, ">");
    } else if (c == ';') {
        t->pos++;
        return new_token(TOK_SEMICOLON, ";");
    } else {
        while (t->input[t->pos] && !strchr("|&><;", t->input[t->pos]) && !isspace(t->input[t->pos])) {
            t->pos++;
            len++;
        }
        char *value = strndup(start, len);
        return new_token(TOK_NAME, value);
    }
}

void free_tokenizer(Tokenizer *t) {
    if (t->current_token) free_token(t->current_token);
    free(t);
}

// Implementation of parser functions
Parser *init_parser(char *input) {
    Parser *p = malloc(sizeof(Parser));
    p->tokenizer = init_tokenizer(input);
    p->tokenizer->current_token = get_next_token(p->tokenizer);
    return p;
}

void free_node(Node *node) {
    if (!node) return;
    switch (node->type) {
        case NODE_SHELL_CMD:
            free_node(node->data.shell_cmd.cmd_group);
            free_node(node->data.shell_cmd.next);
            if (node->data.shell_cmd.connector) free(node->data.shell_cmd.connector);
            break;
        case NODE_CMD_GROUP:
            free_node(node->data.cmd_group.atomic);
            free_node(node->data.cmd_group.next);
            break;
        case NODE_ATOMIC:
            if (node->data.atomic.command) free(node->data.atomic.command);
            if (node->data.atomic.input) free(node->data.atomic.input);
            if (node->data.atomic.output) free(node->data.atomic.output);
            for (int i = 0; i < node->data.atomic.arg_count; i++) {
                free(node->data.atomic.args[i]);
            }
            free(node->data.atomic.args);
            break;
        case NODE_SEQUENTIAL:
            free_node(node->data.sequential.first);
            free_node(node->data.sequential.second);
            break;
    }
    free(node);
}

Token *current_token(Parser *p) {
    return p->tokenizer->current_token;
}

void consume_token(Parser *p, TokenType type) {
    if (current_token(p)->type == type) {
        free_token(p->tokenizer->current_token);
        p->tokenizer->current_token = get_next_token(p->tokenizer);
    } else {
        fprintf(stderr, "Unexpected token: %d, expected: %d\n", current_token(p)->type, type);
    }
}

Node *parse_name(Parser *p) {
    if (current_token(p)->type == TOK_NAME) {
        Node *node = malloc(sizeof(Node));
        node->type = NODE_ATOMIC;
        node->data.atomic.command = strdup(current_token(p)->value);
        node->data.atomic.args = NULL;
        node->data.atomic.arg_count = 0;
        node->data.atomic.input = NULL;
        node->data.atomic.output = NULL;
        node->data.atomic.append = 0;
        consume_token(p, TOK_NAME);
        return node;
    }
    return NULL;
}

Node *parse_input(Parser *p, Node *atomic) {
    if (current_token(p)->type == TOK_RED_IN) {
        consume_token(p, TOK_RED_IN);
        if (current_token(p)->type == TOK_NAME) {
            atomic->data.atomic.input = strdup(current_token(p)->value);
            consume_token(p, TOK_NAME);
        } else {
            fprintf(stderr, "Expected name after <\n");
        }
    }
    return atomic;
}

Node *parse_output(Parser *p, Node *atomic) {
    if (current_token(p)->type == TOK_RED_OUT || current_token(p)->type == TOK_RED_APPEND) {
        atomic->data.atomic.append = (current_token(p)->type == TOK_RED_APPEND);
        consume_token(p, current_token(p)->type);
        if (current_token(p)->type == TOK_NAME) {
            atomic->data.atomic.output = strdup(current_token(p)->value);
            consume_token(p, TOK_NAME);
        } else {
            fprintf(stderr, "Expected name after > or >>\n");
        }
    }
    return atomic;
}

Node *parse_atomic(Parser *p) {
    Node *node = parse_name(p);
    if (!node) return NULL;
    node->data.atomic.args = malloc(sizeof(char *) * 10);
    node->data.atomic.arg_count = 0;
    while (current_token(p)->type == TOK_NAME || current_token(p)->type == TOK_RED_IN ||
           current_token(p)->type == TOK_RED_OUT || current_token(p)->type == TOK_RED_APPEND) {
        if (current_token(p)->type == TOK_NAME) {
            node->data.atomic.args[node->data.atomic.arg_count++] = strdup(current_token(p)->value);
            consume_token(p, TOK_NAME);
        } else if (current_token(p)->type == TOK_RED_IN) {
            node = parse_input(p, node);
        } else {
            node = parse_output(p, node);
        }
    }
    node->data.atomic.args[node->data.atomic.arg_count] = NULL;
    return node;
}

Node *parse_cmd_group(Parser *p) {
    Node *atomic = parse_atomic(p);
    if (!atomic) return NULL;
    Node *cmd_group = malloc(sizeof(Node));
    cmd_group->type = NODE_CMD_GROUP;
    cmd_group->data.cmd_group.atomic = atomic;
    cmd_group->data.cmd_group.next = NULL;
    Node *current = cmd_group;
    while (current_token(p)->type == TOK_PIPE) {
        consume_token(p, TOK_PIPE);
        Node *next_atomic = parse_atomic(p);
        if (!next_atomic) {
            fprintf(stderr, "Expected atomic command after |\n");
        }
        Node *next = malloc(sizeof(Node));
        next->type = NODE_CMD_GROUP;
        next->data.cmd_group.atomic = next_atomic;
        next->data.cmd_group.next = NULL;
        current->data.cmd_group.next = next;
        current = next;
    }
    return cmd_group;
}

Node *parse_shell_cmd(Parser *p) {
    Node *cmd_group = parse_cmd_group(p);
    if (!cmd_group) return NULL;
    Node *shell_cmd = malloc(sizeof(Node));
    shell_cmd->type = NODE_SHELL_CMD;
    shell_cmd->data.shell_cmd.cmd_group = cmd_group;
    shell_cmd->data.shell_cmd.connector = NULL;
    shell_cmd->data.shell_cmd.next = NULL;
    shell_cmd->data.shell_cmd.background = 0;
    Node *current = shell_cmd;
    while (current_token(p)->type == TOK_BG || current_token(p)->type == TOK_AND) {
        current->data.shell_cmd.connector = strdup(current_token(p)->value);
        consume_token(p, current_token(p)->type);
        Node *next_cmd_group = parse_cmd_group(p);
        if (!next_cmd_group) {
            fprintf(stderr, "Expected cmd_group after & or &&\n");
        }
        Node *next = malloc(sizeof(Node));
        next->type = NODE_SHELL_CMD;
        next->data.shell_cmd.cmd_group = next_cmd_group;
        next->data.shell_cmd.connector = NULL;
        next->data.shell_cmd.next = NULL;
        next->data.shell_cmd.background = 0;
        current->data.shell_cmd.next = next;
        current = next;
    }
    if (current_token(p)->type == TOK_BG) {
        shell_cmd->data.shell_cmd.background = 1;
        consume_token(p, TOK_BG);
    }
    return shell_cmd;
}

Node *parse_sequential(Parser *p) {
    Node *first = parse_shell_cmd(p);
    if (!first) return NULL;
    if (current_token(p)->type != TOK_SEMICOLON) {
        return first;
    }
    consume_token(p, TOK_SEMICOLON);
    Node *second = parse_sequential(p);
    if (!second) {
        fprintf(stderr, "Expected command after ;\n");
    }
    Node *node = malloc(sizeof(Node));
    node->type = NODE_SEQUENTIAL;
    node->data.sequential.first = first;
    node->data.sequential.second = second;
    return node;
}

Node *parse(char *input) {
    Parser *p = init_parser(input);
    Node *root = parse_sequential(p);
    if (current_token(p)->type != TOK_EOF) {
        fprintf(stderr, "Unexpected tokens at end of input\n");
        free_node(root);
        root = NULL;
    }
    free_tokenizer(p->tokenizer);
    free(p);
    return root;
}

void print_node(Node *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    switch (node->type) {
        case NODE_SHELL_CMD:
            printf("SHELL_CMD (background=%d)\n", node->data.shell_cmd.background);
            print_node(node->data.shell_cmd.cmd_group, indent + 1);
            if (node->data.shell_cmd.connector) {
                for (int i = 0; i < indent; i++) printf("  ");
                printf("Connector: %s\n", node->data.shell_cmd.connector);
            }
            print_node(node->data.shell_cmd.next, indent);
            break;
        case NODE_CMD_GROUP:
            printf("CMD_GROUP\n");
            print_node(node->data.cmd_group.atomic, indent + 1);
            if (node->data.cmd_group.next) {
                for (int i = 0; i < indent; i++) printf("  ");
                printf("PIPE\n");
            }
            print_node(node->data.cmd_group.next, indent);
            break;
        case NODE_ATOMIC:
            printf("ATOMIC: %s", node->data.atomic.command);
            if (node->data.atomic.arg_count > 0) {
                printf(" (args:");
                for (int i = 0; i < node->data.atomic.arg_count; i++) {
                    printf(" %s", node->data.atomic.args[i]);
                }
                printf(")");
            }
            if (node->data.atomic.input) {
                printf(" < %s", node->data.atomic.input);
            }
            if (node->data.atomic.output) {
                printf(" %s %s", node->data.atomic.append ? ">>" : ">", node->data.atomic.output);
            }
            printf("\n");
            break;
        case NODE_SEQUENTIAL:
            printf("SEQUENTIAL\n");
            print_node(node->data.sequential.first, indent + 1);
            print_node(node->data.sequential.second, indent + 1);
            break;
    }
}
