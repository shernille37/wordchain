
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>

#include "dict.h"
#include "file.h"
#include "utils.h"

void firstChildCompito1(int pipe1[2], int pipe2[2], char * fileName) {

    FILE * pipeRead1 = fdopen(pipe1[0], "r");
    FILE * pipeWrite2 = fdopen(pipe2[1], "w");

    if(pipeRead1 == NULL) {
        perror("Pipe Read Error:");
        exit(EXIT_FAILURE);
    }


    if(pipeWrite2 == NULL) {
        perror("Pipe Write Error:");
        exit(EXIT_FAILURE);
    }

    wchar_t buffer[WORD_SIZE];

    wchar_t firstWord[WORD_SIZE];
    wchar_t prev[WORD_SIZE];

    int first = 1;

    MapPrev * map = initMapPrev();
        
    while(fgetws(buffer, WORD_SIZE, pipeRead1) != NULL) {

        size_t len = wcslen(buffer);
        if(buffer[len - 1] == L'\n') buffer[len - 1] = L'\0';

        if(first) {
            
            wcsncpy(firstWord, buffer, WORD_SIZE);
            wcsncpy(prev, buffer, WORD_SIZE);
            first = 0;
            continue;
        }
        
        insertMapPrev(map, prev, buffer, -1);
        wcsncpy(prev, buffer, WORD_SIZE);

    }

    insertMapPrev(map, buffer, firstWord, -1);


    //setvbuf(pipeWrite2, NULL, _IONBF, 0);

    // TODO SEND DATA TO PROCESS 2
    writeCsv(map, fileName, pipeWrite2);


    fclose(pipeRead1);
    
    fclose(pipeWrite2);

}

void secondChildCompito1(int pipe2[2], char * fileName) {

    FILE * foutCsv;
    FILE * pipeRead2 = fdopen(pipe2[0], "r");

    char * newFileName = changeFileExtension(fileName, "csv");

    if( (foutCsv = fopen(newFileName, "w+")) == NULL ) {
        perror("Error:");
        exit(EXIT_FAILURE);
    }

    wchar_t buffer[WORD_SIZE];

    while(fgetws(buffer, WORD_SIZE, pipeRead2) != NULL) {

        size_t len = wcslen(buffer);
        if(buffer[len - 1] == L'\n') buffer[len - 1] = '\0';

        if(wcsncmp(buffer, L"-1", WORD_SIZE) == 0) fwprintf(foutCsv, L"\n");
        else fwprintf(foutCsv,L"%ls", buffer);
        
    }

    free(newFileName);

    fclose(pipeRead2);  
    fclose(foutCsv);

}

void parentCompito1(int pipe1[2], char * fileName) {

    FILE * pipeWrite1;

    if( (pipeWrite1 = fdopen(pipe1[1], "w")) == NULL) {
        perror("Pipe Write Error:");
        exit(EXIT_FAILURE);
    }

    readFile(NULL, fileName, pipeWrite1);
    fclose(pipeWrite1);
}