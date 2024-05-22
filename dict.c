#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"

// Hash Function for Strings
unsigned int hash(const wchar_t * str, int bucketSize) {
    unsigned int hash = 5381;
    int c;

    while( (c = *str++) ) {
        hash = ( (hash << 5) + hash ) + c;
    }

    return hash % bucketSize;
}

// Initialize MapPrev ( HashMap (<key: string><value: HashMap (<key:string><value:double>) >) )
MapPrev * initMapPrev() {
    MapPrev * mp = malloc(sizeof(MapPrev));
    mp->buckets = (PrevDictionary **)calloc(DEFAULT_SIZE_BUCKET, sizeof(PrevDictionary*));
    mp->nBuckets = DEFAULT_SIZE_BUCKET;
    mp->size = 0;

    return mp;
}

// Function to resize PrevDictionary 
void resizePrevDictionary(MapPrev * mp, int newSize) {

    PrevDictionary ** newBuckets = (PrevDictionary **)calloc(newSize, sizeof(PrevDictionary *));

    for(int i = 0; i < mp->nBuckets; i++) {

        PrevDictionary * currPrevDict = mp->buckets[i];

        while(currPrevDict != NULL) {

            unsigned int newIndex = hash(currPrevDict->word, newSize);
            PrevDictionary * temp_next = currPrevDict->next;

            currPrevDict->next = newBuckets[newIndex];
            newBuckets[newIndex] = currPrevDict;

            currPrevDict = temp_next;

        }

}

    free(mp->buckets);
    mp->buckets = newBuckets;
    mp->nBuckets = newSize;

}

// Function to search for an entry in the HashMap
int searchMapPrev(MapPrev *mp, wchar_t * word) {

    unsigned int index = hash(word, mp->nBuckets);

    PrevDictionary *prevDict = mp->buckets[index];

    while(prevDict != NULL) {
        
        if(wcsncmp(prevDict->word, word, 30 * sizeof(wchar_t)) == 0) return 1;
        

        prevDict = prevDict->next;

    }

    return 0;

}

// Function to insert an entry in the HashMap
void insertMapPrev(MapPrev * mp, wchar_t * prev, wchar_t * next, double prob) {

    // Resize if needed
    if((double)mp->size/mp->nBuckets > 0.7) {
        resizePrevDictionary(mp, 2 * mp->nBuckets);
    }

    unsigned int index = hash(prev, mp->nBuckets);
    PrevDictionary * current = mp->buckets[index];

    while(current != NULL) {

        if(wcsncmp(current->word, prev, 30 * sizeof(wchar_t)) == 0) {
            MapFrequency *currMap = current->frequencyDict;
            insertMapFrequency(currMap, next, prob);
            return;
        }
        
        current = current->next;

    }

    // If prev is not found (Possible Collision)
    PrevDictionary * newDict = malloc(sizeof(PrevDictionary));
    newDict->word = malloc(30 * sizeof(wchar_t));
    newDict->word = wcsdup(prev);
    

    MapFrequency * mapFreq = initMapFrequency();
    insertMapFrequency(mapFreq, next, prob);
    newDict->frequencyDict = mapFreq;

    newDict->next = mp->buckets[index];
    mp->buckets[index] = newDict;
    mp->size++;
}

// Utility function to print the contents of the HashMap 
void printMapPrev(MapPrev * mp) {

    for(int i = 0; i < mp->nBuckets; i++) {
        PrevDictionary *currPrevDict = mp->buckets[i];
        printf("Map Prev Index: %d\n", i);
        while(currPrevDict != NULL) {
            wprintf(L"Prev: %ls\n", currPrevDict->word);

            printMapFrequency(currPrevDict->frequencyDict);
            
            currPrevDict = currPrevDict->next;
        }


    }

}

// Function to free the memory allocated for the dictionary
void freeMapPrev(MapPrev * mp) {

    for(int i = 0; i < mp->nBuckets; i++) {

        PrevDictionary *currPrevDict = mp->buckets[i];

        while(currPrevDict != NULL) {
            PrevDictionary * temp = currPrevDict;

            currPrevDict = currPrevDict->next;

            free(temp->word);
            freeMapFrequency(temp->frequencyDict);
            free(temp);
        }


    }

    free(mp->buckets);
    free(mp);

}


// Initialize MapFrequency HashMap( <key:string> <value: double> )
MapFrequency * initMapFrequency() {

    MapFrequency * map = malloc(sizeof(MapFrequency));
    map->buckets = (Dictionary **)calloc(DEFAULT_SIZE_BUCKET, sizeof(Dictionary *));
    map->nBuckets = DEFAULT_SIZE_BUCKET;
    map->size = 0;
    return map;
    
}

void resizeDictionary(MapFrequency * map, int newSize) {

    Dictionary ** newBuckets = (Dictionary **)calloc(newSize, sizeof(Dictionary *));

    // Iterate over the old buckets and transfer it to the new Buckets;
    for(int i = 0; i < map->nBuckets; i++) {

        Dictionary * current = map->buckets[i];

        while(current != NULL) {

            unsigned int newIndex = hash(current->word, newSize);
            Dictionary * temp_next = current->next;
            
            current->next = newBuckets[newIndex];
            newBuckets[newIndex] = current;

            current = temp_next;

        }


    }

    free(map->buckets);
    map->buckets = newBuckets;
    map->nBuckets = newSize;


}


void insertMapFrequency(MapFrequency * map, wchar_t * word, double prob) {

    // Resize of Needed TODO
    if((double)map->size/map->nBuckets > 0.7) {
        resizeDictionary(map, 2 * map->nBuckets);
    }

    unsigned int index = hash(word, map->nBuckets);

    if(prob == -1) {

        Dictionary * current = map->buckets[index];
        while(current != NULL) {

            // Word Found
            if(wcsncmp(current->word, word, 30 * sizeof(wchar_t)) == 0) {
                current->frequency++;
                return;
            }
            current = current->next;
        }

    }

    // If word is not found
    // Possible collision (Chaining)

    Dictionary * newDict = malloc(sizeof(Dictionary));
    newDict->word = malloc(30 * sizeof(wchar_t));
    newDict->word = wcsdup(word);

    newDict->frequency = prob == -1 ? 1 : prob;

    newDict->next = map->buckets[index];
    map->buckets[index] = newDict;
    map->size++;
}


void printMapFrequency(MapFrequency * map) {

    for(int i = 0; i < map->nBuckets; i++) {
        Dictionary * current = map->buckets[i];
        printf("Index %d: ", i);
        while(current != NULL) {

            wprintf(L"%ls : %f ->", current->word, current->frequency);
            current = current->next;

        }
        printf(" NULL\n");
 }

}

// Function to free the memory allocated for the dictionary
void freeMapFrequency(MapFrequency * map) {

    for(int i = 0; i < map->nBuckets; i++) {

        Dictionary * currDict = map->buckets[i];

        while(currDict != NULL) {

            Dictionary * temp = currDict;
            currDict = currDict->next;
            free(temp->word);
            free(temp);

        }

    }

    free(map->buckets);
    free(map);


}