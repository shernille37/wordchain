#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <string.h>

#include "multiProcess.h"
#include "file.h"
#include "dict.h"
#include "utils.h"

void multiCompito1(char * fileName) {

    pid_t pid1;
    pid_t pid2;
    int pipe1[2];
    int pipe2[2];

    if(pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe Error:");
        exit(EXIT_FAILURE);
    }


    if( (pid1 = fork()) == -1 ) {
        perror("Fork Error:");
        exit(EXIT_FAILURE);
    }

    // First Child Process
    if(pid1 == 0) {

        printf("First Child Process\n");

        close(pipe1[1]); // Close write end of pipe1
        close(pipe2[0]); // Close read end of pipe2

        firstChildCompito1(pipe1, pipe2, fileName);

        close(pipe1[0]);
        close(pipe2[1]);

        exit(EXIT_SUCCESS);

    } else {

        // Parent Process

        close(pipe1[0]);
        
        parentCompito1(pipe1, fileName);

        close(pipe1[1]);


        // Fork Second Child Process
        if( (pid2 = fork()) == -1 ) {
            perror("Fork Error:");
            exit(EXIT_FAILURE);
        }

        // Second Child Process
        if(pid2 == 0) {

            close(pipe1[0]);
            close(pipe1[1]);

            close(pipe2[1]);
            printf("Second Child Process\n\n");

            secondChildCompito1(pipe2, fileName);
            
            close(pipe2[0]);
            exit(EXIT_SUCCESS);

        } else {

            close(pipe2[0]);
            close(pipe2[1]);
            wait(NULL);
            wait(NULL);

        }

    }
}