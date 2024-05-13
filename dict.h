#include <wchar.h>

#define DEFAULT_SIZE_BUCKET 16

// Dictionary Structure
typedef struct Dictionary {
    char * word; // Key
    double frequency; // Value
    struct Dictionary * next;

} Dictionary;

typedef struct {
    Dictionary ** buckets;
    int nBuckets;
    int size;

} MapFrequency; 

// TODO 

typedef struct PrevDictionary {
    char * word; // Key
    MapFrequency * frequencyDict; // Value
    struct PrevDictionary * next;
    
} PrevDictionary;

typedef struct {
    PrevDictionary ** buckets;
    int nBuckets;
    int size;
} MapPrev;

unsigned int hash(const char * str, int bucketSize);

MapFrequency * initMapFrequency();
void resizeDictionary(MapFrequency * map, int newSize);
void insertMapFrequency(MapFrequency * map, char * word, double prob);
void printMapFrequency(MapFrequency * map);
void freeMapFrequency(MapFrequency *dict);

MapPrev * initMapPrev();
void resizePrevDictionary(MapPrev * mp, int newSize);
int searchMapPrev(MapPrev *mp, char * word);
void insertMapPrev(MapPrev * mp, char * prev, char * next, double prob);
void printMapPrev(MapPrev * mp);
void freeMapPrev(MapPrev * mp);


