#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_TOKENS 256
#define MAX_LEN 1024

// Token types
typedef enum {
    T_NAME,
    T_AMP,      // &
    T_ANDAND,   // &&
    T_PIPE,     // |
    T_INPUT,    // <  or <name
    T_OUTPUT,   // >  or >name
    T_APPEND,   // >> or >>name
    T_EOF
} TokenType;

typedef struct {
    TokenType type;
    char text[MAX_LEN];
} Token;

Token tokens[MAX_TOKENS];
int pos, token_count;

// --- Lexer ---
void add_token(TokenType type, const char *start, int len) {
    tokens[token_count].type = type;
    strncpy(tokens[token_count].text, start, len);
    tokens[token_count].text[len] = '\0';
    token_count++;
}

void tokenize(const char *input) {
    token_count = 0;
    const char *p = input;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '&') {
            if (*(p+1) == '&') {
                add_token(T_ANDAND, p, 2);
                p += 2;
            } else {
                add_token(T_AMP, p, 1);
                p++;
            }
        }
        else if (*p == '|') {
            add_token(T_PIPE, p, 1);
            p++;
        }
        else if (*p == '<') {
            add_token(T_INPUT, p, 1);
            p++;
        }
        else if (*p == '>') {
            if (*(p+1) == '>') {
                add_token(T_APPEND, p, 2);
                p += 2;
            } else {
                add_token(T_OUTPUT, p, 1);
                p++;
            }
        }
        else {
            // NAME
            const char *start = p;
            while (*p && !isspace(*p) && *p != '|' && *p != '&' && *p != '>' && *p != '<')
                p++;
            add_token(T_NAME, start, p - start);
        }
    }
    add_token(T_EOF, "", 0);
}

// --- Parser ---
Token *peek() { return &tokens[pos]; }
Token *advance() { return &tokens[pos++]; }
bool match(TokenType type) {
    if (peek()->type == type) {
        advance();
        return true;
    }
    return false;
}

// Forward declarations
bool shell_cmd();
bool cmd_group();
bool atomic();
bool name();

// name -> r"[^|&><;]+"
bool name() {
    return match(T_NAME);
}

// input -> < name | <name
bool input() {
    if (match(T_INPUT)) {
        return name();
    }
    return false;
}

// output -> > name | >name | >> name | >>name
bool output() {
    if (match(T_OUTPUT) || match(T_APPEND)) {
        return name();
    }
    return false;
}

// atomic -> name (name | input | output)*
bool atomic() {
    if (!name()) return false;
    while (name() || input() || output()) {
        // keep consuming
    }
    return true;
}

// cmd_group -> atomic (\| atomic)*
bool cmd_group() {
    if (!atomic()) return false;
    while (match(T_PIPE)) {
        if (!atomic()) return false;
    }
    return true;
}

// shell_cmd -> cmd_group ((& | &&) cmd_group)* &?
bool shell_cmd() {
    if (!cmd_group()) return false;
    while (match(T_AMP) || match(T_ANDAND)) {
        if (!cmd_group()) return false;
    }
    match(T_AMP); // optional trailing &
    return true;
}

bool parse() {
    pos = 0;
    bool ok = shell_cmd();
    return ok && peek()->type == T_EOF;
}

// --- Main loop ---
int main() {
    char line[MAX_LEN];

    while (1) {
        printf("myshell> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        tokenize(line);
        if (parse()) {
            printf("valid\n");
        } else {
            printf("not valid\n");
        }
    }
    return 0;
}

