#include <stdio.h>

void toLowerString(wchar_t * str);
char * getFileExtension(char * filename);
char * changeFileExtension(char * filename, char * newExtension);
char * pickRandomSeparator();
void removeZeros(char * str);
int valid_identifier_start(char ch);
void ifFirst(wchar_t ** firstword, int * first, wchar_t * word);
int randomIndexGenerator(double * probs, int length);
