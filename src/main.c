#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "prompt.h"
#include "parser.h"

int main(){
    get_prompt_info();

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while(true){
        prompt_printer();

        nread = getline(&line, &len, stdin);
        if (nread == -1) {

            if (feof(stdin)) {

                printf("\n");
                break;
            } else {
                perror("getline");
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

        Node *root = parse(line);
        if(root){
            print_node(root, 0);
            free_node(root);
        }
    }

    free(line);
    prompt_cleaner();
    return 0;
}
