#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum{
    CONN_NONE,
    CONN_SEQ,   // ;
    CONN_ANDAND,   // &&
    CONN_AND     // &
} ConnectorType;

typedef struct{
    char *argv[16];
    int argcount;
    char *input;	// < file
    char *output;	// > file
    int append;		// 1 for >>
} Atomic;

typedef struct{
    Atomic atomics[16];
    int atomic_count;
    ConnectorType connector; // connector to next block
    int background;

} CommandBlock;

typedef struct{
    CommandBlock blocks[16];
    int blockcount;
} ParsedLine;


static char *read_token(const char **p){
	
    while(**p && isspace((unsigned char)**p))
    	(*p)++;

    if (**p == '\0') return NULL;

    const char *start = *p;
    
    if(start[0] == '>' && start[1] == '>'){
        *p += 2;
        return strdup(">>");
    }
    
    if(start[0] == '&' && start[1] == '&'){
        *p += 2;
        return strdup("&&");
    }

    if(strchr("|&;<>", start[0])){
        (*p)++;
        return strndup(start, 1);
    }

    char buf[1024];
    size_t i = 0;

    if(**p == '"' || **p == '\''){
        char quote = **p;
        buf[i++] = *(*p)++; // dont skip the quote
        while(**p && **p != quote){
            buf[i++] = *(*p)++;
        }
        if(**p == quote)
        	buf[i++] = *(*p)++;
    }
    else{
        while(**p && !isspace((unsigned char)**p) && !strchr("|&;<>", **p)){
            buf[i++] = *(*p)++;
        }
    }

    buf[i] = '\0';
    return strdup(buf);
}

bool filename_check(char *s){

    if(!s || !*s){
    	return false;
    }
    while(*s){
        if (*s == '|' || *s == '&' || *s == '>' || *s == '<' || *s == ';')
            return false;
        s++;
    }
    return true;
}

ParsedLine parse_line(const char *line){

    ParsedLine pl = {0};
    pl.blockcount = 1;
    CommandBlock *block = &pl.blocks[0];
    block->atomic_count = 1;
    block->background = 0;
    Atomic *atomic = &block->atomics[0];

    const char *p = line;
    char *token;

    while((token = read_token(&p)) != NULL){

		// I'm hndling piping and other connectionshere
        if(strcmp(token, "|") == 0){
            // starting a new atomic in same block
            atomic = &block->atomics[block->atomic_count];
            block->atomic_count++;
        }
        else if(strcmp(token, "&&") == 0){
            block->connector = CONN_ANDAND;
            block = &pl.blocks[pl.blockcount];
            pl.blockcount++;
            block->atomic_count = 1;
            block->background = 0;
            atomic = &block->atomics[0];
        }
        else if(strcmp(token, ";") == 0) {
            block->connector = CONN_SEQ;
            block = &pl.blocks[pl.blockcount];
            pl.blockcount++;
            block->atomic_count = 1;
            block->background = 0;
            atomic = &block->atomics[0];
        }
        else if(strcmp(token, "&") == 0){
            block->connector = CONN_AND;
            block->background = 1;
            block = &pl.blocks[pl.blockcount];
            pl.blockcount++;
            block->atomic_count = 1;
            block->background = 0; 
            atomic = &block->atomics[0];
            
        }
		else if(strcmp(token, "<") == 0){
		    char *file = read_token(&p);
		    if(!filename_check(file)){
		    	file = "";
		    }
		    atomic->input = file;
		}
		else if(strcmp(token, ">") == 0 || strcmp(token, ">>") == 0){
		    char *file = read_token(&p);
		    if(!filename_check(file)){
		    	file = "";
		    }
		    atomic->output = file;
		    atomic->append = (token[1] == '>');
		}
        else{
            atomic->argv[atomic->argcount++] = token;
            token = NULL; // prevent free
        }

        if(token)
        	free(token);
    }

    return pl;
}
