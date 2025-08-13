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

            if (i == parsedline->blockcount - 1 && i > 0 &&
                parsedline->blocks[i-1].connector == CONN_AND) {
                // Remove the last block
                parsedline->blockcount--;
                // Mark previous block as background
                parsedline->background = 1;
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
                (a->output && strlen(a->output) == 0)) {
                printf("Error: Missing file for redirection.\n");
                return 1;
            }
        }
    }

    return 0;
}
