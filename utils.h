#include <stdio.h>

void toLowerString(wchar_t * str);
char * getFileExtension(char * filename);
char * changeFileExtension(char * filename, char * newExtension);
wchar_t * pickRandomSeparator();
void removeZeros(char * str);
void ifFirst(wchar_t ** firstword, int * first, wchar_t * word);
int randomIndexGenerator(double * probs, int length);
wchar_t * readLine(FILE *stream);
