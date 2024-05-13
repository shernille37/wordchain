#include <string.h>
#include <ctype.h>
#include "dict.h"
#include "file.h"
#include "utils.h"


// State functionalities
// State 1  ?,!,.
void STATE_1(int * state, char * word, int *i, char c, MapPrev * mp, char * prev, char ** firstWord, int *first) {

    if(c == L'?' || c == L'!' || c == L'.') {
        word[(*i)++] = c;
        word[*i] = '\0';
        
        ifFirst(firstWord, first, word);
        insertMapPrev(mp, prev, word, -1);
        strcpy(prev, word);

        (*i) = 0; 

    }
    else if(isalpha(c) || c == L'\''){
        (*state) = 3;
        word[(*i)++] = c;
    }
    else (*state) = 2;

}



// State 2 Separator
void STATE_2(int * state,char * word, int *i, char c, MapPrev * mp, char * prev, char ** firstWord, int *first) {

    if(c == L'?' || c == L'!' || c == L'.') {

        (*state) = 1;
        word[(*i)++] = c;
        word[(*i)] = '\0';

        ifFirst(firstWord, first, word);
        insertMapPrev(mp, prev, word, -1);
        strcpy(prev, word);

        (*i) = 0; 

    } else if(isalpha(c) || c == L'\'') {
        (*state) = 3;
        word[(*i)++] = c;
    } 
    else return;

}



// State 3 Letter
void STATE_3(int * state,char * word, int *i, char c, MapPrev * mp, char * prev, char ** firstWord, int *first) {

    if(c == L'?' || c == L'!' || c == L'.') {
        (*state) = 1;
        word[(*i)] = '\0';
        toLowerString(word);

        ifFirst(firstWord, first, word);
        insertMapPrev(mp, prev, word, -1);

        strcpy(prev, word);
        (*i) = 0;

        word[(*i)++] = c;
        word[(*i)] = '\0';
        insertMapPrev(mp, prev, word, -1);
        strcpy(prev, word);
        (*i) = 0;

    } 
    else if(isalpha(c) || c == L'\'') word[(*i)++] = c;
    else {
        (*state) = 2;
        word[(*i)] = '\0';
        toLowerString(word);
        ifFirst(firstWord, first, word);

        insertMapPrev(mp, prev, word, -1);
        strcpy(prev, word);
        (*i) = 0;
    } 

}
