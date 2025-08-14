#include <stdio.h>
#include "parser.h"

const char* conn_type(ConnectorType type, const ParsedLine* parsedline ){
    switch(type){
        case 0:  return "NONE";
        case 1:  return "SEQUENTIAL";
        case 2:  return "&&";
        case 3:
			if(parsedline->background==1)
        		return "& [BACKGROUND]";
        	else
        		return "& [CONNECTOR]";
        default: return "UNKNOWN";
    }
}

int print_cmd(ParsedLine *parsedline){

    for (int b = 0; b < parsedline->blockcount; b++){
        CommandBlock *block = &parsedline->blocks[b];

        printf("=== Block %d ===\n", b);
        printf("Connector: %s\n", conn_type(block->connector, parsedline));

        for (int a = 0; a < block->atomic_count; a++) {
            const Atomic *atomic = &block->atomics[a];

            if(atomic->argcount > 0){
                printf("Command: %s\n", atomic->argv[0]);

                if(atomic->argcount > 1){
                    printf("Arguments: ");
                    for(int i = 1; i < atomic->argcount; i++){
                        printf("%s ", atomic->argv[i]);
                    }
                    printf("\n");
                } else {
                    printf("Arguments: (none)\n");
                }
            } else {
                printf("Empty atomic command\n");
            }
        }

        printf("\n");
    }
    return 0;
}
