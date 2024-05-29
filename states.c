#include <string.h>
#include <ctype.h>
#include "dict.h"
#include "file.h"
#include "utils.h"

// State functionalities Finite State Machine

/*
State 1: Punctuations (. ! ?)

*/

void STATE_1(int * state, wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t * firstWord, int *first, FILE * pipe) {

    // If it encounters a punctuation
    if(c == L'?' || c == L'!' || c == L'.') {
        word[0] = c;
        word[1] = L'\0';
        
        if(*first) {
            firstWordHandler(firstWord, prev, word, first, i);
            return;
        }

        if(pipe) fwprintf(pipe, L"%ls\n", word);
        else insertMapPrev(mp, prev, word, -1);

        wcsncpy(prev, word, WORD_SIZE);

        (*i) = 0; 

    }
    else if(iswalpha(c) || c == L'\'' || c == L'’'){ // If it's a valid letter
        (*state) = 3;
        word[(*i)++] = c;
    }
    else (*state) = 2; // If it's a separator

}



// State 2 Separator
void STATE_2(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t * firstWord, int *first, FILE * pipe) {

    // If it encounter punctuation
    if(c == L'?' || c == L'!' || c == L'.') {

        (*state) = 1;
        word[0] = c;
        word[1] = L'\0';

        if(*first) {
            firstWordHandler(firstWord, prev, word, first, i);
            return;
        }

        if(pipe) fwprintf(pipe, L"%ls\n", word);
        else insertMapPrev(mp, prev, word, -1);

        wcsncpy(prev, word, WORD_SIZE);

        (*i) = 0; 

    } else if(iswalpha(c) || c == L'\'' || c == L'’') { // If it's a word
        (*state) = 3;
        word[(*i)++] = c;
    } 
    else return;

}

// State 3 Letter
void STATE_3(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t * firstWord, int *first, FILE * pipe) {

    // If it's a punctuation
    if(c == L'?' || c == L'!' || c == L'.') {
        (*state) = 1;
        word[(*i)] = L'\0';
        toLowerString(word);

         if(*first) {
            firstWordHandler(firstWord, prev, word, first, i);
            
            word[0] = c;
            word[1] = L'\0';

            if(pipe) fwprintf(pipe, L"%ls\n", word);
            else insertMapPrev(mp, prev, word, -1);
                        
            wcsncpy(prev, word, WORD_SIZE);
            (*i) = 0;

            return;
        }

        if(pipe) fwprintf(pipe, L"%ls\n", word);
        else insertMapPrev(mp, prev, word, -1);

        wcsncpy(prev, word, WORD_SIZE);
        (*i) = 0;

        word[0] = c;
        word[1] = L'\0';

        if(pipe) fwprintf(pipe, L"%ls\n", word);
        else insertMapPrev(mp, prev, word, -1);

        wcsncpy(prev, word, WORD_SIZE);


    } 
    else if(iswalpha(c)) word[(*i)++] = c; // If it's a valid letter
    else { // If it's a separator
        (*state) = 2;

        if(c == L'\'' || c == L'’') {
            word[(*i)++] = c;
            word[(*i)] = L'\0';
        } else
            word[(*i)] = L'\0';

        toLowerString(word);

        if(*first) {
            firstWordHandler(firstWord, prev, word, first, i);
            return;
        }


        if(pipe) fwprintf(pipe, L"%ls\n", word);
        else insertMapPrev(mp, prev, word, -1);
        
        wcsncpy(prev, word, WORD_SIZE);
        (*i) = 0;
    } 

}