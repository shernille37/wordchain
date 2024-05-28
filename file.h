#ifndef FILE_H
#define FILE_H

#include <wchar.h>
#include <stdlib.h>
#include "dict.h"

#define WORD_SIZE 30 * sizeof(wchar_t) // Maximum word size as stated in the requisite

// -- COMPITO 1 --

// Read text file provided in input
void readFile(MapPrev * mp, char * fileName, FILE * pipe);

// Write CSV file in output
void writeCsv(MapPrev *dict, char * filename, FILE * pipe);

// Manages the STATES (Finite State Machine) for reading the file in input
void STATE_1(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t * firstWord, int *first, FILE * pipe);
void STATE_2(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t * firstWord, int *first, FILE * pipe);
void STATE_3(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t * firstWord, int *first, FILE * pipe);

// -- COMPITO 2 --

// Read CSV file in input
void readCsv(MapPrev *mp, char * filename, FILE * pipe);

// Generate the random text in output
void produceText(MapPrev *mp, int nWords, char * prevWord, FILE * pipe);


#endif