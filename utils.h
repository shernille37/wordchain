#ifndef UTILS_H
#define UTILS_H

#include <wchar.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

void toLowerString(wchar_t * str);
char * getFileExtension(char * filename);
char * changeFileExtension(char * filename, char * newExtension);
wchar_t * pickRandomSeparator(MapPrev *mp, gsl_rng * r);
void removeZeros(char * str);
void firstWordHandler(wchar_t * firstword, wchar_t * prev, wchar_t * word, int * first, int * index);
wchar_t * readLine(FILE *stream);

#endif
