#ifndef DICT_H
#define DICT_H

#include <wchar.h>
#define DEFAULT_SIZE_BUCKET 16      // Initial BUCKET SIZE for the HashMap

/*
In this project I basically used:
2 HashMap structures:

- MapFrequency: count the frequency of words
- MapPrev: store the previous words and words immediately after

Collisions are managed using the Chaining Method(Linked List):

*/



/*
(Key, Value) Entry for the HashMap
Key: Word
Value: Frequency
*/
typedef struct Dictionary {
    wchar_t * word; // Key
    double frequency; // Value
    struct Dictionary * next; // Pointer to the next entry

} Dictionary;

/*
HashMap Structure to store the frequency of words in the input file.
It stores the metadata of the HashMap
*/
typedef struct {
    Dictionary ** buckets; // Array of Linked List (Buckets)
    int nBuckets; // Number for Buckets
    int size; // Load size of the HashMap
} MapFrequency; 

/*
(Key, Value) Entry for the HashMap
Key: Previous Word
Value: Pointer to the HashMap that contains Word Frequencies
*/
typedef struct PrevDictionary {
    wchar_t * word; // Key
    MapFrequency * frequencyDict; // Value
    struct PrevDictionary * next; // Pointer to the next entry

} PrevDictionary;

/*
HashMap Structure to store the previous words and the words immediately after
It stores the metadata of the HashMap
*/
typedef struct {
    PrevDictionary ** buckets; // Array of Linked List (Buckets)
    int nBuckets; // Number of Buckets
    int size; // Load size
} MapPrev;


// Hash Function
unsigned int hash(const wchar_t * str, int bucketSize);

/*
Functions of the HashMap:
- Resize 
- Insertion
- Search
- Free Map
*/
MapFrequency * initMapFrequency();
void resizeDictionary(MapFrequency * map, int newSize);
void insertMapFrequency(MapFrequency * map, wchar_t * word, double prob);
void printMapFrequency(MapFrequency * map);
void freeMapFrequency(MapFrequency *dict);

MapPrev * initMapPrev();
void resizePrevDictionary(MapPrev * mp, int newSize);
int searchMapPrev(MapPrev *mp, wchar_t * word);
void insertMapPrev(MapPrev * mp, wchar_t * prev, wchar_t * next, double prob);
void printMapPrev(MapPrev * mp);
void freeMapPrev(MapPrev * mp);


#endif
