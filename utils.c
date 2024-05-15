#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <wchar.h>
#include "utils.h"

#define BUFFER_SIZE 1024

int valid_identifier_start(char ch)
{
    return ((ch >= L'A' && ch <= L'Z') || (ch >= L'a' && ch <= L'z') ||
            ( ch >= 0xc0));
}

wchar_t * readLine(FILE *stream)
{
    wchar_t * buffer = NULL;
    size_t size = 0;
    wchar_t * newline_found = NULL;  
 
    do
    {   
        
        if( (buffer = realloc(buffer, size + BUFSIZ))  == NULL) {
            perror("Error: ");
            exit(EXIT_FAILURE);
        }
        // End of file reached
        if ( (fgetws(buffer + size, BUFSIZ, stream)) == NULL) {
            return buffer;
        }

        // Check if newline is reached
        newline_found = wcschr(buffer + size, L'\n');
    } while (!newline_found && (size += BUFSIZ)); // Re-iterate until newline is not reached or EOF is reached

 
    return buffer;
}

char * getFileExtension(char * filename) {

    char * dot = strrchr(filename, '.');

    if(!dot || dot == filename) return "";
    return dot + 1;

}

char * changeFileExtension(char * filename, char * newExtension) {

    char * dot = strrchr(filename, '.');

    // Remove current file extension
    if(dot != NULL) *dot = '\0';
    

    char * newFileName;

    if( (newFileName = malloc(strlen(filename) + strlen(newExtension) + 2)) == NULL ) {
        perror("Error:");
        exit(EXIT_FAILURE);
    }


    strcat(newFileName, filename);
    strcat(newFileName, ".");
    strcat(newFileName, newExtension);

    return newFileName;

}

wchar_t * pickRandomSeparator() {

    srand(time(0));
    // Pick random number between 0 and 1
    double random;

    for(int i = 0; i < 3; i ++)random = (double )rand() / (double )RAND_MAX;
    
    wchar_t *separators[] = {L"!", L".", L"?"};
    double probs[] = {0.333333, 0.333333, 0.333333};

    
    double cum_prob = 0;
    for(int i = 0; i < 3; i++) {
        cum_prob += probs[i];
        if(random < cum_prob) 
            return separators[i];
    }

    // It shouldn't arrive here
    return NULL;

}

void removeZeros(char * str) {
    int len = strlen(str);
    while (len-- && (str[len - 1] == '0' || str[len - 1] == '.')){
        if(str[len - 1] == '.') {
            str[len - 1] = '\0';
            return;
        }
        str[len - 1] = '\0';
    } 
}

void toLowerString(wchar_t * str) {

    for(int i = 0; i < wcslen(str); i++) {
        str[i] = towlower(str[i]);  
    }

}
void ifFirst(wchar_t ** firstword, int * first, wchar_t * word) {

    if((*first)) {
        (*firstword) = wcsdup(word);
        (*first) = 0;

    }

}


int randomIndexGenerator(double * probs, int length) {

    // Normalize the probabilities
    double sum = 0;
    for(int i = 0; i < length; i++) sum += probs[i];
    for(int i = 0; i < length; i++) probs[i] /= sum;
    // This line generates a random double  between 0 and 1

    double random = (double )rand() / (double )RAND_MAX;
    double cum_prob = 0;
    for (int i = 0; i < length; i++) {
        cum_prob += probs[i];
        if (random < cum_prob)
            return i;
    }

    return -1;
}
