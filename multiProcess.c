#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <string.h>

#include "multiProcess.h"
#include "file.h"
#include "dict.h"
#include "utils.h"

/*
This function holds the structure of the multi-process version of the project
in both of Compito1 and Compito2

3 forked processes:

Parent Process (Main): Reader Process (Reads the files in input)

First Child: Constructor Process (Constructs the Output)

Second Child: Printer Process (Prints the output based on the constructed output)


*/

void multiProcessi(char * fileName, int compito1Flag, int nWords, char * prevWord) {

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

        close(pipe1[1]); 
        close(pipe2[0]); 

        if(compito1Flag) firstChildCompito1(pipe1, pipe2, fileName);
        else firstChildCompito2(pipe1, pipe2, fileName, nWords, prevWord);

        close(pipe1[0]);
        close(pipe2[1]);

        exit(EXIT_SUCCESS);

    } else {

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
            
            if(compito1Flag) secondChildCompito1(pipe2, fileName);
            else secondChildCompito2(pipe2, fileName);
            
            close(pipe2[0]);
            exit(EXIT_SUCCESS);

        } else {
            // Parent Process

            close(pipe1[0]);
            close(pipe2[0]);
            close(pipe2[1]);

            if(compito1Flag) parentCompito1(pipe1, fileName);
            else parentCompito2(pipe1, fileName);

            close(pipe1[1]);

            // Wait for the 2 children processes to finish
            wait(NULL);
            wait(NULL);

        }

    }
}