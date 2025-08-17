#include <stdio.h>
#include "parser.h"
#include "cmdarchive.h"

const char* conn_type(ConnectorType type){
    switch(type){
        case 0:  return "NONE";
        case 1:  return "; SEQUENTIAL";
        case 2:  return "&& SEQUENTIAL";
        case 3:  return "& BACKGROUND";
        default: return "UNKNOWN";
    }
}

int print_cmd(ParsedLine *parsedline){

    for (int b = 0; b < parsedline->blockcount; b++){
        CommandBlock *block = &parsedline->blocks[b];

        printf("==== BLOCK %d ====\n", b);
        printf("Block Connector: %s\n", conn_type(block->connector));
		printf("Background: %d\n", block->background);
        for(int a = 0; a < block->atomic_count; a++){
            const Atomic *atomic = &block->atomics[a];

			if(a!=0)
	        	printf("---- PIPE ----\n");		
            
           	const CommandInfo* cmd = get_cmd_info(atomic->argv[0]);
			if(cmd==NULL){
				printf("Invalid command: %s\n", atomic->argv[0]);
				continue;
			}

            if(atomic->argcount > 0){
                printf("Command: %s\nInfo: %s\n", cmd->name, cmd->help);

                if(atomic->argcount > 1){
                    printf("Arguments: ");
                    for(int i = 1; i < atomic->argcount; i++){
                        printf("%s | ", atomic->argv[i]);
                    }
                    printf("\n");
                }
				else{
					printf("Arguments: (none)\n");
				}
                
				if(atomic->input)
					printf("[input <] %s\n", atomic->input);
				if(atomic->output)
					printf("%s %s\n", atomic->append ? "[append >>]" : "[output >]", atomic->output);
            }
            else{
                printf("Empty atomic command\n");
            }
        }
    }
    printf("-----------------\n");
    return 0;
}


void dispatch_cmd(ParsedLine *parsedline){
	printf("To dispatch the parsed line \n");
}
