#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "cmdarchive.h"
#include "hop_util.h"
#include "reveal_util.h"
#include "log_util.h"
#include "activities_util.h"
#include "ping_util.h"
#include "fgbg_util.h"

const char* conn_type(ConnectorType type){
    switch(type){
        case 0:  return "NONE";
        case 1:  return "; SEQUENTIAL";
        case 2:  return "&& SEQUENTIAL";
        case 3:  return "& BACKGROUND";
        default: return "UNKNOWN";
    }
}

void print_helper(CommandBlock *block){
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
	                printf("%s", atomic->argv[i]);
	                if(i!=atomic->argcount-1)
	                	printf(" | ");
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


int print_cmd(ParsedLine *parsedline){

    for(int b = 0; b < parsedline->blockcount; b++){
        CommandBlock *block = &parsedline->blocks[b];

        printf("==== BLOCK %d ====\n", b);
        printf("Block Connector: %s\n", conn_type(block->connector));
		printf("Background: %d\n", block->background);
		print_helper(block);
    }
  	printf("-----------------\n");
    return 0;
}

int run_block(CommandBlock *block){

	printf("[EXEC] Running block with %d pipes...\n", block->atomic_count-1);
	print_helper(block);
	printf("----------------------------------PRINTER DONE\n");
	for(int a = 0; a < block->atomic_count; a++){
	    const Atomic *atomic = &block->atomics[a];

			if(a!=0)
	        	printf("---- PIPING ----\n");		
	    
	   	const CommandInfo* cmd = get_cmd_info(atomic->argv[0]);
		if(cmd==NULL){
			printf("Invalid command: %s\n", atomic->argv[0]);
			continue;
		}
		if(strcmp(cmd->name,"hop")==0){
			cmd_hop(atomic);
		}
		else if(strcmp(cmd->name,"reveal")==0){
			cmd_reveal(atomic);
		}
		else if(strcmp(cmd->name,"log")==0){
			cmd_log(atomic);
		}
		else if(strcmp(cmd->name,"activities")==0){
			cmd_activities(atomic);
		}
		else if(strcmp(cmd->name,"ping")==0){
			cmd_ping(atomic);
		}
		else if(strcmp(cmd->name,"fg")==0){
			cmd_fg(atomic);
		}
		else if(strcmp(cmd->name,"bg")==0){
			cmd_bg(atomic);
		}
	}
    return 0;
}

int run_block_bg(CommandBlock *block){

   	printf("[EXEC BG] Running block with %d pipes in the background...\n", block->atomic_count-1);
   	print_helper(block);
   	printf("----------------------------------PRINTER DONE\n\n");
   	for(int a = 0; a < block->atomic_count; a++){
   	    const Atomic *atomic = &block->atomics[a];
   
   			if(a!=0)
   	        	printf("---- PIPING ----\n");		
   	    
   	   	const CommandInfo* cmd = get_cmd_info(atomic->argv[0]);
   		if(cmd==NULL){
   			printf("Invalid command: %s\n", atomic->argv[0]);
   			continue;
   		}
   		if(strcmp(cmd->name,"hop")==0){
   			cmd_hop(atomic);
   		}
   		else if(strcmp(cmd->name,"reveal")==0){
   			cmd_reveal(atomic);
   		}
   		else if(strcmp(cmd->name,"log")==0){
   			cmd_log(atomic);
   		}
   		else if(strcmp(cmd->name,"activities")==0){
   			cmd_activities(atomic);
   		}
   		else if(strcmp(cmd->name,"ping")==0){
   			cmd_ping(atomic);
   		}
   		else if(strcmp(cmd->name,"fg")==0){
   			cmd_fg(atomic);
   		}
   		else if(strcmp(cmd->name,"bg")==0){
   			cmd_bg(atomic);
   		}
   	}
       return 0;
}


int dispatch_cmd(ParsedLine *parsedline){
    int last_status = 0;

    for(int b = 0; b < parsedline->blockcount; b++){
    
        CommandBlock *block = &parsedline->blocks[b];
        
        printf("==== BLOCK %d ====\n", b);
        printf("Block Connector: %s\n", conn_type(block->connector));
		printf("Background: %d\n", block->background);

		if(block->connector == CONN_AND || block->background == 1){
		    last_status = 0;
		    run_block_bg(block);
		}
		/*
		else if(block->connector == CONN_ANDAND){

		    if(last_status == 0){
		        last_status = run_block(block);
		    } 
		    else{
		        printf("[&&] Skipping block %d (prev failed)\n", b);
		        break;
		    }
		}
		*/
		else if(block->connector == CONN_SEQ || block->connector == CONN_ANDAND){
		    last_status = run_block(block);
		    last_status = 0;
		}
		else{
		    run_block(block);
		    last_status = 0;
		}
	}
    return last_status;
}
