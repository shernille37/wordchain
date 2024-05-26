
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>

#include "dict.h"
#include "file.h"

void firstChildCompito1(int pipe1[2], int pipe2[2], char * fileName) {

    FILE * pipeRead = fdopen(pipe1[0], "r");
    wchar_t buffer[WORD_SIZE];

    wchar_t firstWord[WORD_SIZE];
    wchar_t prev[WORD_SIZE];

    int first = 1;

    MapPrev * map = initMapPrev();
        
    while(fgetws(buffer, WORD_SIZE, pipeRead) != NULL) {

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

    writeCsv(map, fileName);
    // TODO SEND DATA TO PROCESS 2
    write(pipe2[1], "HI PROCESS 2\0", 1024);

    fclose(pipeRead);

}

void parentCompito1(int pipe1[2], char * fileName) {

    FILE * pipeWrite;

    if( (pipeWrite = fdopen(pipe1[1], "w")) == NULL) {
        perror("Pipe Write Error:");
        exit(EXIT_FAILURE);
    }

    readFile(NULL, fileName, pipeWrite);

    fclose(pipeWrite);
}