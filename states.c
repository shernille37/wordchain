#include <string.h>
#include <ctype.h>
#include "dict.h"
#include "file.h"
#include "utils.h"


// State functionalities
// State 1  ?,!,.
void STATE_1(int * state, wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t ** firstWord, int *first) {

    if(c == L'?' || c == L'!' || c == L'.') {
        word[(*i)++] = c;
        word[*i] = '\0';
        
        ifFirst(firstWord, first, word);
        insertMapPrev(mp, prev, word, -1);
        wcscpy(prev, word);

        (*i) = 0; 

    }
    else if(iswalpha(c)){
        (*state) = 3;
        word[(*i)++] = c;
    }
    else (*state) = 2;

}



// State 2 Separator
void STATE_2(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t ** firstWord, int *first) {

    if(c == L'?' || c == L'!' || c == L'.') {

        (*state) = 1;
        word[(*i)++] = c;
        word[(*i)] = '\0';

        ifFirst(firstWord, first, word);
        insertMapPrev(mp, prev, word, -1);
        wcscpy(prev, word);

        (*i) = 0; 

    } else if(iswalpha(c)) {
        (*state) = 3;
        word[(*i)++] = c;
    } 
    else return;

}



// State 3 Letter
void STATE_3(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t ** firstWord, int *first) {

    if(c == L'?' || c == L'!' || c == L'.') {
        (*state) = 1;
        word[(*i)] = '\0';
        toLowerString(word);

        ifFirst(firstWord, first, word);
        insertMapPrev(mp, prev, word, -1);

        wcscpy(prev, word);
        (*i) = 0;

        word[(*i)++] = c;
        word[(*i)] = '\0';
        insertMapPrev(mp, prev, word, -1);
        wcscpy(prev, word);
        (*i) = 0;

    } 
    else if(iswalpha(c)) word[(*i)++] = c;
    else {
        (*state) = 2;
        word[(*i)] = '\0';
        toLowerString(word);
        ifFirst(firstWord, first, word);

        insertMapPrev(mp, prev, word, -1);
        wcscpy(prev, word);
        (*i) = 0;
    } 

}
