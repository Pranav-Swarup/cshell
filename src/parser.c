#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum{
    CONN_NONE,
    CONN_SEQ,   // ;
    CONN_AND,   // &&
    CONN_BG     // &
} ConnectorType;

typedef struct {
    char *argv[16];
    int argcount;
    char *input;	// < file
    char *output;	// > file
    int append;		// 1 for >>
} Atomic;

typedef struct {
    Atomic atomics[16];
    int atomic_count;
    ConnectorType connector; // connector to next block
} CommandBlock;

typedef struct {
    CommandBlock blocks[16];
    int blockcount;
    int background;
} ParsedLine;


void print_parsed(const ParsedLine *pl){

    printf("Parsed %d blocks\n", pl->blockcount);
    
    for(int i = 0; i < pl->blockcount; i++){
    
        printf(" Block %d: connector = %d\n", i, pl->blocks[i].connector);

        for(int j = 0; j < pl->blocks[i].atomic_count; j++){
        
            const Atomic *a = &pl->blocks[i].atomics[j];
            printf("   Atomic %d: ", j);

            for(int k = 0; k < a->argcount; k++){
                printf("%s ", a->argv[k]);
            }
            
            if(a->input)
            	printf("[input] %s ", a->input);
            if(a->output)
            	printf("%s %s ", a->append ? "[append >>]" : "[output >]", a->output);

            printf("\n");
        }
    }
}

static char *read_token(const char **p){

    while(**p && isspace((unsigned char)**p))
    	(*p)++;

    if (**p == '\0') return NULL;

    const char *start = *p;

    if (start[0] == '>' && start[1] == '>') {
        *p += 2;
        return strdup(">>");
    }
    
    if (start[0] == '&' && start[1] == '&') {
        *p += 2;
        return strdup("&&");
    }

    if (strchr("|&;<>", start[0])) {
        (*p)++;
        return strndup(start, 1);
    }

    while(**p && !isspace((unsigned char)**p) && !strchr("|&;<>", **p)){
        (*p)++;
    }
    return strndup(start, *p - start);
}

char* filename_check(char *s) {
    if(!s || !*s){
    	return "";
    }
    while(*s){
        if (*s == '|' || *s == '&' || *s == '>' || *s == '<' || *s == ';')
            return "";
        s++;
    }
    return s;
}

ParsedLine parse_line(const char *line){

    ParsedLine pl = {0};
    pl.blockcount = 1;
    CommandBlock *block = &pl.blocks[0];
    block->atomic_count = 1;
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
            block->connector = CONN_AND;
            block = &pl.blocks[pl.blockcount];
            pl.blockcount++;
            block->atomic_count = 1;
            atomic = &block->atomics[0];
        }
        else if (strcmp(token, ";") == 0) {
            block->connector = CONN_SEQ;
            block = &pl.blocks[pl.blockcount];
            pl.blockcount++;
            block->atomic_count = 1;
            atomic = &block->atomics[0];
        }
        else if (strcmp(token, "&") == 0){
            block->connector = CONN_BG;
            block = &pl.blocks[pl.blockcount];
            pl.blockcount++;
            block->atomic_count = 1;
            atomic = &block->atomics[0];
        }
		else if(strcmp(token, "<") == 0){
		    char *file = read_token(&p);
		    file = filename_check(file);
		    atomic->input = file;
		}
		else if(strcmp(token, ">") == 0 || strcmp(token, ">>") == 0){
		    char *file = read_token(&p);
		    file = filename_check(file);
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
