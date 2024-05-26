#ifndef DICT_H
#define DICT_H

#include <wchar.h>
#define DEFAULT_SIZE_BUCKET 16

// Dictionary Structure
typedef struct Dictionary {
    wchar_t * word; // Key
    double frequency; // Value
    struct Dictionary * next;

} Dictionary;

typedef struct {
    Dictionary ** buckets;
    int nBuckets;
    int size;

} MapFrequency; 

typedef struct PrevDictionary {
    wchar_t * word; // Key
    MapFrequency * frequencyDict; // Value
    struct PrevDictionary * next;

} PrevDictionary;

typedef struct {
    PrevDictionary ** buckets;
    int nBuckets;
    int size;
} MapPrev;

unsigned int hash(const wchar_t * str, int bucketSize);

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
