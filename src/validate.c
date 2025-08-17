#include <stdio.h>
#include <string.h>
#include "parser.h"

void rm_emptyblocks(ParsedLine *parsedline){

    for(int i = 0; i < parsedline->blockcount;){

        int empty = 0;

        if(parsedline->blocks[i].atomic_count == 0 ||
            parsedline->blocks[i].atomics[0].argcount == 0) {
            empty = 1;
        }

        if(empty){
			if(i == parsedline->blockcount - 1 && i > 0 &&
                parsedline->blocks[i-1].connector == CONN_AND){
                parsedline->blockcount--;
                //parsedline->blocks[i-1].background = 1; Already being marked in parser.c
                parsedline->blocks[i-1].connector = CONN_NONE;
            } 
            else{
                for(int j = i; j < parsedline->blockcount - 1; j++){
                    parsedline->blocks[j] = parsedline->blocks[j+1];
                }
                parsedline->blockcount--;
                continue;  // dont move to next 'i' chck from current block.
            }
        }
        i++;
    }
}

int quotes_checker(const char *s){
    int in_single = 0, in_double = 0;
    while(*s){
        if(*s == '\'' && !in_double){
            in_single = !in_single;
        } 
        else if(*s == '"' && !in_single){
            in_double = !in_double;
        }
        s++;
    }
    return (in_single || in_double);
}

char check_invalid_char(const char *str){
    while(*str){
        if (*str == '|' || *str == '&' || *str == '>' || *str == '<' || *str == ';') {
            return *str;
        }
        str++;
    }
    return 'k';
}

int validate_input(ParsedLine *parsedline){

	rm_emptyblocks(parsedline);

	if(parsedline->blockcount == 0){
	    printf("Error: Empty command\n");
	    return 1;
	}
	
    for(int i = 0; i < parsedline->blockcount; i++){

        const CommandBlock *block = &parsedline->blocks[i];

        if(i == parsedline->blockcount - 1){
            if(block->connector == CONN_ANDAND || block->connector == CONN_SEQ){
                printf("Error: Line cannot end with connector ; or &&\n");
                return 1;
            }
        }

        for(int j = 0; j < block->atomic_count; j++){
            const Atomic *a = &block->atomics[j];

            if(a->argcount == 0 && !a->input && !a->output){
                printf("Error: Empty command detected\n");
                return 1;
            }
            if((a->input && strlen(a->input) == 0) ||
                (a->output && strlen(a->output) == 0)){
                printf("Error: Missing file for redirection.\n");
                return 1;
            }

			// Checking quotes in raw input and in the filenames
            
			for(int k = 0; k < a->argcount; k++){
			    if(quotes_checker(a->argv[k])){
		            printf("Error: Unclosed quotes in argument: %s\n", a->argv[k]);
		            return 1;
			    }

				if((a->argv[k][0] == '"'  && a->argv[k][1] == '"' ) ||
				    (a->argv[k][0] == '\'' && a->argv[k][1] == '\'')){
				    printf("Error: Quotes empty in argument: %s\n", a->argv[k]);
				    return 1;
				}
				if(a->argv[k][0] == '"' || a->argv[k][0] == '\''){
					char temp = check_invalid_char(a->argv[k]);
					if(temp != 'k'){
						printf("Error: Quotes contain invalid char: %c in argument: %s\n", temp, a->argv[k]);
						return 1;
					}
				}
			}

		    if(a->input && quotes_checker(a->input)){
		        printf("Error: Unclosed quotes in input filename: %s\n", a->input);
		        return 1;
		    }
		    if(a->output && quotes_checker(a->output)){
		        printf("Error: Unclosed quotes in output filename: %s\n", a->output);
		        return 1;
		    }
		}
    }

    return 0;
}
