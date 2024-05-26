#ifndef FILE_H
#define FILE_H


#include <wchar.h>
#include <stdlib.h>
#include "dict.h"

#define WORD_SIZE 30 * sizeof(wchar_t)

void readFile(MapPrev * mp, char * fileName, FILE * pipe);
void writeCsv(MapPrev *dict, char * filename);

void STATE_1(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t * firstWord, int *first, FILE * pipe);
void STATE_2(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t * firstWord, int *first, FILE * pipe);
void STATE_3(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t * firstWord, int *first, FILE * pipe);

void readCsv(MapPrev *mp, char * filename);
void produceText(MapPrev *mp, int nWords, char * prevWord);


#endif