#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"


/*
Function to Hash Strings (djb2)
- Written by Daniel J. Bernstein

Input: 
- Wide String String
- Bucket Size

Output:
- hash % bucketSize  -- Indicates the index of the bucket for access

*/
unsigned int hash(const wchar_t * str, int bucketSize) {
    unsigned int hash = 5381;
    int c;

    while( (c = *str++) ) {
        hash = ( (hash << 5) + hash ) + c;
    }

    return hash % bucketSize;
}

/*
Initialize MapPrev HashMap structure

Dynamically allocates memory for every metadata of the HashMap

*/ 
MapPrev * initMapPrev() {
    MapPrev * mp = malloc(sizeof(MapPrev));
    mp->buckets = (PrevDictionary **)calloc(DEFAULT_SIZE_BUCKET, sizeof(PrevDictionary*));
    mp->nBuckets = DEFAULT_SIZE_BUCKET;
    mp->size = 0;

    return mp;
}


/*
Resizes the MapPrev HashMap acoordingly if the load factor exceeds a certain threshold (0.7)
Load Factor = size/nBuckets

Used to preserve the constant complexity operations of the HashMap. 
    O(1 + alpha) where alpha is the load factor

It rehashes the old entries of the Buckets and transfers it to the new Buckets
It updates the metadata of the HashMap


Input:
- Instance of the Map
- New Size of the Map

*/
void resizePrevDictionary(MapPrev * mp, int newSize) {

    // Dynamically allocate new buckets for the storage of the old entries
    PrevDictionary ** newBuckets = (PrevDictionary **)calloc(newSize, sizeof(PrevDictionary *));

    // Iterate through the old map
    for(int i = 0; i < mp->nBuckets; i++) {

        PrevDictionary * currPrevDict = mp->buckets[i];

        while(currPrevDict != NULL) {
            
            // Hash the key(prev word) proportional to the new size of the map
            unsigned int newIndex = hash(currPrevDict->word, newSize);

            PrevDictionary * temp_next = currPrevDict->next;

            // Store it to the New Buckets allocated
            currPrevDict->next = newBuckets[newIndex];
            newBuckets[newIndex] = currPrevDict;

            currPrevDict = temp_next;

        }

}

    // Free the old buckets
    free(mp->buckets);

    // Updates the metadata
    mp->buckets = newBuckets;
    mp->nBuckets = newSize;

}

/*
Function to search for an entry in the MapPrev

Input:
-- Instance of the Map
-- Word to search for

Output:
-- 1    if the word is found
-- 0    otherwise

*/

int searchMapPrev(MapPrev *mp, wchar_t * word) {

    // Hash the word to search
    unsigned int index = hash(word, mp->nBuckets);

    // Get the Head of the linked list in the bucket
    PrevDictionary *prevDict = mp->buckets[index];

    // Iterate through the linked list until the word is found
    while(prevDict != NULL) {
        
        // Compare the searched word
        if(wcsncmp(prevDict->word, word, 30 * sizeof(wchar_t)) == 0) return 1;
        

        prevDict = prevDict->next;

    }

    // Word not found
    return 0;

}

/*

Function to insert an entry in the HashMap

Input:
-- Instance of the Map
-- Previous Word (MapPrev Key)

-- Next Word (MapFrequency Key)
-- Probability (MapFrequency Value) [Optional]

*/

void insertMapPrev(MapPrev * mp, wchar_t * prev, wchar_t * next, double prob) {

    // Check if the Map exceeded the threshold (0.7) and resize if needed
    if((double)mp->size/mp->nBuckets > 0.7) {
        resizePrevDictionary(mp, 2 * mp->nBuckets);
    }

    // Hash the key
    unsigned int index = hash(prev, mp->nBuckets);
    PrevDictionary * current = mp->buckets[index];

    // Iterate through the list
    while(current != NULL) {

        // If the key (prev word) is found: Update the value (MapFrequency)
        if(wcsncmp(current->word, prev, 30 * sizeof(wchar_t)) == 0) {
            MapFrequency *currMap = current->frequencyDict;
            insertMapFrequency(currMap, next, prob);
            return;
        }
        
        current = current->next;

    }

    // If prev is not found (Possible Collision)
    // Create a new Entry for the map
    PrevDictionary * newDict = malloc(sizeof(PrevDictionary));
    newDict->word = malloc(30 * sizeof(wchar_t));
    newDict->word = wcsdup(prev); // Key
    
    // Value
    MapFrequency * mapFreq = initMapFrequency();
    insertMapFrequency(mapFreq, next, prob);
    newDict->frequencyDict = mapFreq;

    // Append the entry to the head of the list
    newDict->next = mp->buckets[index];
    mp->buckets[index] = newDict;

    mp->size++; // Update the size
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


/*
Initialize MapFrequency HashMap structure

Dynamically allocates memory for every metadata of the HashMap

*/ 
MapFrequency * initMapFrequency() {

    MapFrequency * map = malloc(sizeof(MapFrequency));
    map->buckets = (Dictionary **)calloc(DEFAULT_SIZE_BUCKET, sizeof(Dictionary *));
    map->nBuckets = DEFAULT_SIZE_BUCKET;
    map->size = 0;
    return map;
    
}

// Resize the Map
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


/*
Insert an entry into the MapFreq 

If the prob parameter is -1
It means that it is not present
*/
void insertMapFrequency(MapFrequency * map, wchar_t * word, double prob) {

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

// Utility function to print the MapFrequency
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