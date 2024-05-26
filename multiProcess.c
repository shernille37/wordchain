#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <string.h>

#include "multiProcess.h"
#include "file.h"
#include "dict.h"

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

        close(pipe1[1]);
        close(pipe2[0]);

        firstChildCompito1(pipe1, pipe2, fileName);

        close(pipe1[0]);
        close(pipe2[1]);
        exit(EXIT_SUCCESS);

    } else {

        // Parent Process (Read File)
        close(pipe1[0]);
        
        parentCompito1(pipe1, fileName);

        close(pipe1[1]);

        // Wait for first child to finish before forking the second child process
        wait(NULL);

        // Fork Second Child Process
        if( (pid2 = fork()) == -1 ) {
            perror("Fork Error:");
            exit(EXIT_FAILURE);
        }

        // Second Child Process
        if(pid2 == 0) {
            close(pipe2[1]);
            printf("Second Child Process\n\n");

            char buffer[1024];
            read(pipe2[0], buffer, 1024);

            printf("Second Child Received: %s\n", buffer);

            close(pipe2[0]);
            exit(EXIT_SUCCESS);

        } else {
            wait(NULL);
        }

    }
}