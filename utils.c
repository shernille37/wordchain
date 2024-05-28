#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <wchar.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "dict.h"
#include "utils.h"
#include "file.h"

/*

Reads an arbitrarily long line in a file (CSV)

Input:
-- File Steam

Output:
-- Wide Character pointer (Line)

*/
wchar_t * readLine(FILE *stream) {
    wchar_t *buffer = NULL;  // Pointer to hold the dynamically allocated line
    size_t size = 0;           // Tracks the current size of the buffer (in wchar_t elements)

    wchar_t *newline_found = NULL;  // Pointer to store the location of the newline character

    do {
        // Reallocate the buffer to accommodate more characters (initial allocation is 0)
        if ((buffer = realloc(buffer, (size + BUFSIZ) * sizeof(wchar_t))) == NULL) {
            perror("Error: ");
            exit(EXIT_FAILURE);  // Exit on realloc failure
        }

        // Check for end-of-file (fgetws returns NULL on EOF)
        if (fgetws(buffer + size, BUFSIZ, stream) == NULL) {
            return buffer;  // If EOF, return the current buffer (may or may not have a newline)
        }

        // Search for newline character within the newly read portion
        newline_found = wcschr(buffer + size, '\n');
    } while (!newline_found && (size += BUFSIZ)); // Loop until newline is found or EOF is reached

    return buffer;
}

/*
    Get file extension

    Input:
    -- Filename

    Output:
    -- Extension of the filename

*/
char * getFileExtension(char * filename) {

    // Searches for a dot and return its pointer
    char * dot = strrchr(filename, '.');

    if(!dot || dot == filename) return "";
    return dot + 1; // Return pointer + 1 (Pointer to the start of the extension)

}

/*
    Change file Extension

    Input:
    -- Filename
    -- New Extension

    Output:
    -- Pointer to the new File Extention (String)

*/

char * changeFileExtension(char * filename, char * newExtension) {

    // Searches for a dot
    char * dot = strrchr(filename, '.');

    // Remove current file extension
    if(dot != NULL) *dot = '\0';
    

    char * newFileName;

    // Dynamically allocate a memory for the new filename
    if( (newFileName = malloc(strlen(filename) + strlen(newExtension) + 2)) == NULL ) {
        perror("Error:");
        exit(EXIT_FAILURE);
    }

    // Construct the new filename through concatenation
    strcat(newFileName, filename); // Concatenate old filename
    strcat(newFileName, "."); // Concatenate "dot"
    strcat(newFileName, newExtension); // Concatenate new extension

    return newFileName;

}
/*
    Pick a random separator

    Input:
    -- Instance of MapPrev
    -- GSL Random Number Generator Instance

    Output:
    -- Wide Char pointer to the Separator Picked

*/
wchar_t * pickRandomSeparator(MapPrev *mp, gsl_rng *r) {
    // Array of possible separator characters
    wchar_t *separators[] = {L"!", L".", L"?"};

    // Variables to store filtered separators and their probabilities
    wchar_t **presentSeparators = NULL;
    double *probs = NULL;

    int counter = 0;

    // Find all separators present in the text based on the MapPrev data structure
    for (int i = 0; i < 3; i++) {
        if (searchMapPrev(mp, separators[i])) {
            // Reallocate memory for presentSeparators to store the found separator
            presentSeparators = (wchar_t **)realloc(presentSeparators, (counter + 1) * sizeof(wchar_t *));
            presentSeparators[counter++] = separators[i];
        }
    }

    // Handle the case where no separators are found in the text
    if (counter == 0) {
        printf("There are no punctuations to choose from in the text!\n");
        exit(EXIT_FAILURE);
    }

    // Allocate and assign equal probabilities to each present separator
    probs = (double *)realloc(probs, (counter) * sizeof(double));
    for (int i = 0; i < counter; i++) {
        probs[i] = (double)1 / counter;
    }

    // Create a GSL random discrete distribution using the probabilities
    gsl_ran_discrete_t *dist = gsl_ran_discrete_preproc(counter, probs);

    // Sample a random index from the discrete distribution
    int index = gsl_ran_discrete(r, dist);

    // Pick the separator at the chosen index from presentSeparators
    wchar_t *pickedSeparator = presentSeparators[index];

    // Clean up memory
    gsl_ran_discrete_free(dist);
    free(presentSeparators);
    free(probs);

    // Return the randomly picked separator character
    return pickedSeparator;
}

/*
    Remove Zeros from a String

    Input:
    -- String (Number)

*/
void removeZeros(char * str) {
    int len = strlen(str);

    /*
        Iterate from the end of the string until it reaches NON zero or a dot
    */
    while (len-- && (str[len - 1] == '0' || str[len - 1] == '.')){
        if(str[len - 1] == '.') {
            str[len - 1] = '\0';
            return;
        }
        str[len - 1] = '\0';
    } 
}

// Transform a string to LOWER case
void toLowerString(wchar_t * str) {

    // Iterate through the string and use the towlower function
    for(int i = 0; i < wcslen(str); i++) {
        str[i] = towlower(str[i]);  
    }

}

// First word handler to handle every first word in a File
void firstWordHandler(wchar_t * firstword, wchar_t * prev, wchar_t * word, int * first, int * index) {
    wcsncpy(firstword, word, WORD_SIZE);
    wcsncpy(prev, word, WORD_SIZE);
    (*first) = 0;
    (*index) = 0;
    
}
