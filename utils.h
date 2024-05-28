#ifndef UTILS_H
#define UTILS_H

#include <wchar.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


/*
Utilities Functions used in the entire Project
*/

// Transform the string to LowerCase
void toLowerString(wchar_t * str);

// Get the file extension of a Filename
char * getFileExtension(char * filename);

// Change the file extension of a Filename
char * changeFileExtension(char * filename, char * newExtension);

// Remove Zeroes in the string preserving significant digits
void removeZeros(char * str);

// Pick a random separator among the separators present in the text (. ! ?)
wchar_t * pickRandomSeparator(MapPrev *mp, gsl_rng * r);

// Handle every first word encountered while reading the input file
void firstWordHandler(wchar_t * firstword, wchar_t * prev, wchar_t * word, int * first, int * index);

// Read an arbitrarily large line in the CSV file
wchar_t * readLine(FILE *stream);

#endif
