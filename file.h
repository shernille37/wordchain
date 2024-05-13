void readFile(MapPrev * mp, char * fileName);
void writeCsv(MapPrev *dict, char * filename);

void STATE_1(int * state,char * word, int *i, char c, MapPrev * mp, char * prev, char ** firstWord, int *first);
void STATE_2(int * state,char * word, int *i, char c, MapPrev * mp, char * prev, char ** firstWord, int *first);
void STATE_3(int * state,char * word, int *i, char c, MapPrev * mp, char * prev, char ** firstWord, int *first);

void readCsv(MapPrev *mp, char * filename);
void produceText(MapPrev *mp, int nWords, char * prevWord);
