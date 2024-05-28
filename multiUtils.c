
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>

#include "dict.h"
#include "file.h"
#include "utils.h"

/*
Function of the First Child for Compito 1

Populates the MapPrev for construction of the output CSV file

Input:
-- Pipe1 (Communication between parent and first child)
-- Pipe2 (Communication between first child and second child)
-- Filename provided in input via command line

*/

void firstChildCompito1(int pipe1[2], int pipe2[2], char * fileName) {

    // Open the both pipes
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
    
    /*
        Reads the pipe line by line sent by the parent process
    */

    while(fgetws(buffer, WORD_SIZE, pipeRead1) != NULL) {

        // Remove the newline character
        size_t len = wcslen(buffer);
        if(buffer[len - 1] == L'\n') buffer[len - 1] = L'\0';

        // If it's the first word, put it into the firstWord variable and continue
        if(first) {
            
            wcsncpy(firstWord, buffer, WORD_SIZE);
            wcsncpy(prev, buffer, WORD_SIZE);
            first = 0;
            continue;
        }
        
        // Insert the entry to the MAP
        insertMapPrev(map, prev, buffer, -1);
        wcsncpy(prev, buffer, WORD_SIZE);

    }

    insertMapPrev(map, buffer, firstWord, -1);

    // Send the CSV format to second child for printing
    writeCsv(map, fileName, pipeWrite2);

    fclose(pipeRead1);
    
    fclose(pipeWrite2);

}

/*
Function of the Second Child for Compito 1

Prints the Output based on the words sent by First Child

Input:
-- Pipe2 (Communication between first child and second child)
-- Filename provided in input via command line

*/

void secondChildCompito1(int pipe2[2], char * fileName) {

    FILE * foutCsv;
    FILE * pipeRead2 = fdopen(pipe2[0], "r");

    // Change the file extension to be .csv file
    char * newFileName = changeFileExtension(fileName, "csv");

    if( (foutCsv = fopen(newFileName, "w+")) == NULL ) {
        perror("Error:");
        exit(EXIT_FAILURE);
    }

    wchar_t buffer[WORD_SIZE];
    
    // Read the words sent by the first child for printing
    while(fgetws(buffer, WORD_SIZE, pipeRead2) != NULL) {

        size_t len = wcslen(buffer);
        if(buffer[len - 1] == L'\n') buffer[len - 1] = '\0';

        // If it reads -1, it means the it reached the end and needs to print a newline
        if(wcsncmp(buffer, L"-1", WORD_SIZE) == 0) fprintf(foutCsv, "\n");
        else fwprintf(foutCsv, L"%ls", buffer);
        
    }

    free(newFileName);

    fclose(pipeRead2);  
    fclose(foutCsv);

}

/*
Function of the Parent Process for Compito 1

Reads the input file and sends it to the first child

Input:
-- Pipe1 (Communication between parent process and first child)
-- Filename provided in input via command line

*/


void parentCompito1(int pipe1[2], char * fileName) {

    FILE * pipeWrite1;

    if( (pipeWrite1 = fdopen(pipe1[1], "w")) == NULL) {
        perror("Pipe Write Error:");
        exit(EXIT_FAILURE);
    }

    // Read the file in input and send it to first child
    readFile(NULL, fileName, pipeWrite1);
    fclose(pipeWrite1);

}

// --- Functions COMPITO 2 ---

/*
Function of the Parent Process for Compito 2

Reads the input CSV file and sends it to the first child

Input:
-- Pipe1 (Communication between parent process and first child)
-- Filename provided in input via command line

*/
void parentCompito2(int pipe1[2], char * fileName) {

    FILE * pipeWrite1;

    if( (pipeWrite1 = fdopen(pipe1[1], "w")) == NULL) {
        perror("Pipe Write Error:");
        exit(EXIT_FAILURE);
    }

    // Read the file in input and send it to first child
    readCsv(NULL, fileName, pipeWrite1);
    
    fclose(pipeWrite1);

}

/*
Function of the First Child for Compito 2

Populates the MapPrev for construction of the output random text file

Input:
-- Pipe1 (Communication between parent and first child)
-- Pipe2 (Communication between first child and second child)
-- Filename provided in input via command line

*/
void firstChildCompito2(int pipe1[2], int pipe2[2], char * fileName, int nWords, char * prevWord) {

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

    wchar_t prev[WORD_SIZE];
    wchar_t word[WORD_SIZE];
    double prob;

    int first = 1;

    MapPrev * map = initMapPrev();

    /*
        Read the words sent by parent line by line

        Uses a binary counter

        Counter 0: Word
        Counter 1: Probability
    
    */

    int counter = 0;
    while(fgetws(buffer, WORD_SIZE, pipeRead1) != NULL) {

        size_t len = wcslen(buffer);
        if(buffer[len - 1] == L'\n') buffer[len - 1] = L'\0';

        if(first) {
            wcsncpy(prev, buffer, WORD_SIZE);
            first = 0;
            continue;
        }

        // If it reads -1 it means that it reached the end of the CSV file
        if(wcsncmp(buffer, L"-1", WORD_SIZE) == 0) {first = 1; continue;}
            
        if(!counter){wcsncpy(word, buffer, WORD_SIZE); counter++;}
        else {
            prob = wcstod(buffer, NULL);
            insertMapPrev(map, prev, word, prob);   // Insert it into the HashMap
            counter = 0;
        }

    }

    // Send generated words for printing to second child
    produceText(map, nWords, prevWord, pipeWrite2);
    
    fclose(pipeRead1);
    fclose(pipeWrite2);

}

/*
Function of the Second Child for Compito 2

Prints the Output based on the words sent by First Child

Input:
-- Pipe2 (Communication between first child and second child)
-- Filename provided in input via command line

*/

void secondChildCompito2(int pipe2[2], char * fileName) {
    
    FILE * pipeRead2 = fdopen(pipe2[0], "r");

    if(pipeRead2 == NULL) {
        perror("Pipe Read Error");
        exit(EXIT_FAILURE);
    }

    FILE * output;

    wchar_t buffer[WORD_SIZE];

    if((output = fopen("output.txt", "w+")) == NULL) {
        perror("File Creation Error:");
        exit(EXIT_FAILURE);
    }

    /*
        Read the words sent by the second child line by line
    */
    while(fgetws(buffer, WORD_SIZE, pipeRead2) != NULL) {

        size_t len = wcslen(buffer);

        if(buffer[len - 1] == L'\n') buffer[len - 1] = L'\0';

        // If it reads -1 it means that the next word needs to be attached to the previous word
        if(wcsncmp(buffer, L"-1", WORD_SIZE) == 0) {
            fseek(output, -1, SEEK_END); // Set the cursor of the file -1 from the end of file
            continue;
        }

        fwprintf(output, L"%ls", buffer);

    }

    fclose(pipeRead2);
    fclose(output);

}