#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "redir.h"

static int saved_stdin  = -1;
static int saved_stdout = -1;

int apply_redirection(Atomic *atomic){

    if(saved_stdin == -1){
    
        saved_stdin = dup(STDIN_FILENO);
        saved_stdout = dup(STDOUT_FILENO);
        if(saved_stdin < 0 || saved_stdout < 0){
            printf("dup");
            return -1;
        }
    }

    if(atomic->input){
    
        int filedesc = open(atomic->input, O_RDONLY);

        if(filedesc < 0){
            printf("No such file or directory\n");
            return -1;
        }

        if(dup2(filedesc, STDIN_FILENO) < 0){
            perror("dup2");
            close(filedesc);
            return -1;
        }

        close(filedesc);

		
	
        
    }

    

    if(atomic->output){
        int flags = O_WRONLY | O_CREAT;
        flags = flags | (atomic->append ? O_APPEND : O_TRUNC);

        int filedesc = open(atomic->output, flags, 0644);
        if(filedesc < 0){
            printf("Cannot open output file: %s\n", atomic->output);
            return -1;
        }
        if(dup2(filedesc, STDOUT_FILENO) < 0){
            perror("dup2");
            close(filedesc);
            return -1;
        }
        close(filedesc);
    }

    return 0;
}

void restore_redirection(){

	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdin);
	saved_stdin = -1;
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdout);
	saved_stdout = -1;
}
