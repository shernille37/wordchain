#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <wchar.h>
#include "dict.h"
#include "utils.h"
#include "file.h"


wchar_t * readLine(FILE *stream)
{
    wchar_t * buffer = NULL;
    size_t size = 0;
    wchar_t * newline_found = NULL;  
 
    do
    {   
        
        if( (buffer = realloc(buffer, (size + BUFSIZ) * sizeof(wchar_t)))  == NULL) {
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

wchar_t * pickRandomSeparator(MapPrev *mp) {

    srand(time(0));
    // Pick random number between 0 and 1
    double random;

    for(int i = 0; i < 3; i ++)random = (double )rand() / (double )RAND_MAX;
    
    wchar_t *separators[] = {L"!", L".", L"?"};

    wchar_t ** presentSeparators = NULL;
    double *probs = NULL;

    int counter = 0;
    for(int i = 0; i < 3; i++) {
        if(searchMapPrev(mp, separators[i])) {
            presentSeparators = (wchar_t **)realloc(presentSeparators, (counter + 1) * sizeof(wchar_t *));
            presentSeparators[counter++] = separators[i];
        }
    }

    if(counter == 0) {
        printf("There are no punctuations to choose from in the text!\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < counter; i++) {
        probs = realloc(probs, (i + 1) * sizeof(double));
        probs[i] = (double)1/counter;
    }

    double cum_prob = 0;
    for(int i = 0; i < counter; i++) {
        cum_prob += probs[i];
        if(random < cum_prob) return presentSeparators[i];
    }

    free(presentSeparators);
    free(probs);


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
void firstWordHandler(wchar_t * firstword, wchar_t * prev, wchar_t * word, int * first, int * index) {
    wcsncpy(firstword, word, WORD_SIZE);
    wcsncpy(prev, word, WORD_SIZE);
    (*first) = 0;
    (*index) = 0;
    
}


int randomIndexGenerator(double * probs, int length) {


    // Normalize the probabilities
    double sum = 0;
    for(int i = 0; i < length; i++) sum += probs[i];
    for(int i = 0; i < length; i++) probs[i] /= sum;
    // This line generates a random double  between 0 and 1
    double random;
    for(int i = 0; i < length; i++) random = (double )rand() / (double )RAND_MAX;

    double cum_prob = 0;
    for (int i = 0; i < length; i++) {
        cum_prob += probs[i];
        if (random < cum_prob)
            return i;
    }

    return -1;
}
