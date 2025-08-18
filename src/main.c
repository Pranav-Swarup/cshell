#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "prompt.h"
#include "parser.h"
#include "validate.h"
#include "cmdrunner.h"
#include "cmdarchive.h"

int main(){
    get_prompt_info();

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while(true){
        prompt_printer();

        nread = getline(&line, &len, stdin);
        if(nread == -1){

            if(feof(stdin)){

                printf("\n");
                break;
            } 
            else{
                printf("Error in getline");
                break;
            }
        }

		if(nread > 0 && line[nread - 1] == '\n'){
            line[nread - 1] = '\0';
            nread--;
        }

        if(nread == 0){
            continue;
        }

		ParsedLine parsed = parse_line(line);
		if(validate_input(&parsed)==1){
			continue;
		}
		dispatch_cmd(&parsed);
    }

    free(line);
    prompt_cleaner();
    return 0;
}
