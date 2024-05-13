void readFile(MapPrev * mp, char * fileName);
void writeCsv(MapPrev *dict, char * filename);

void STATE_1(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t ** firstWord, int *first);
void STATE_2(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t ** firstWord, int *first);
void STATE_3(int * state,wchar_t * word, int *i, wchar_t c, MapPrev * mp, wchar_t * prev, wchar_t ** firstWord, int *first);

void readCsv(MapPrev *mp, char * filename);
void produceText(MapPrev *mp, int nWords, char * prevWord);
