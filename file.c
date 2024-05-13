#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <ctype.h>
#include "dict.h"
#include "file.h"
#include "utils.h"

void readFile(MapPrev * mp, char * fileName) {

    FILE *fp;

    // Open file
    if((fp = fopen(fileName, "r")) == NULL) {
        perror("File Error");
        exit(0);
    }
    
    wchar_t *word = malloc(30 * sizeof(wchar_t));

    int first = 1;
    wchar_t *firstWord = malloc(30 * sizeof(wchar_t));

    // Previous String
    wchar_t *prev = malloc(30 * sizeof(wchar_t)); 
    wcscpy(prev, L".");

    int i = 0;
    wint_t c;

    int state = 1;

    while((c = fgetwc(fp) ) != EOF) {
        c = (wchar_t) c;

        if(state == 1) STATE_1(&state,word, &i, c, mp, prev, &firstWord, &first);
        else if(state == 2) STATE_2(&state,word, &i, c, mp, prev, &firstWord, &first);
        else STATE_3(&state,word, &i, c, mp, prev, &firstWord, &first);

    }

    if(state == 1) {
        word[i+1] = '\0';
        insertMapPrev(mp, word, firstWord, -1);
    } else if (state == 2) {
        toLowerString(word);
        insertMapPrev(mp, word, firstWord , -1);
    }

    else if(state == 3) {
        word[i] = '\0';
        toLowerString(word);
        insertMapPrev(mp, prev, word , -1);
        insertMapPrev(mp, word, firstWord, -1);
    }

    free(word);
    free(firstWord);
    free(prev);

    return;
}

void writeCsv(MapPrev *mp, char * filename) {

    FILE *fout;
    char * newFileName = changeFileExtension(filename, "csv");

    if( (fout = fopen(newFileName, "w+")) == NULL ) {
        perror("Error:");
        exit(EXIT_FAILURE);
    }

    int countMapPrev = 0;
    for(int i = 0; i < mp->nBuckets; i++) {

        PrevDictionary * currPrevDict = mp->buckets[i];


        while(currPrevDict != NULL) {
            fprintf(fout, "%s,", currPrevDict->word);

            // Compute Sum
            double sum = 0;
            char n[20];

            MapFrequency * currMapFreq = currPrevDict->frequencyDict;

            // Calculate sum
            for(int j = 0; j < currMapFreq->nBuckets; j++) {   

                Dictionary * currFreqDict = currMapFreq->buckets[j];

                while(currFreqDict != NULL) {
                    sum += currFreqDict->frequency;
                    currFreqDict = currFreqDict->next;
                }
            }

            //Output words
            int countMapFreq = 0;
            for(int j = 0; j < currMapFreq->nBuckets; j++) {

                Dictionary * currFreqDict = currMapFreq->buckets[j];

                while(currFreqDict != NULL) {
                    fprintf(fout, "%s,", currFreqDict->word);

                    double prob = currFreqDict->frequency/sum;
                    //Convert the probability into a string to easily modify its format
                    sprintf(n, "%f", prob);

                    // Remove trailing zeros while maintaining the significant digits
                    removeZeros(n);
                    fprintf(fout, "%s", n);
                    
                    countMapFreq++;

                    if(countMapFreq < currMapFreq->size) fprintf(fout, ",");
                    currFreqDict = currFreqDict->next;
                }
            }


            countMapPrev++;
            if(countMapPrev < mp->size) fprintf(fout, "\n");
            
            currPrevDict = currPrevDict->next;

        }

    }

    free(newFileName);
    fclose(fout);

}


void readCsv(MapPrev *mp, char * filename) {

    FILE *csvFile;

    // Check if the filename provided is a CSV file
    if( strcmp( getFileExtension(filename), "csv" ) != 0 ) {
        printf("The filename shoud be a CSV file!\n");
        exit(EXIT_FAILURE);
    }

    if( (csvFile = fopen(filename, "r")) == NULL) {
        perror("Error");
        exit(EXIT_FAILURE);
    }

    char * prev = malloc(30);
    char * word = malloc(30);
    double prob;


    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while( (read = getline(&line, &len, csvFile)) != -1 ) {

        if(line[read - 1] == '\n') line[read - 1] = '\0';

        char * token = strtok(line, ",");
        prev = strdup(token);

        token = strtok(NULL, ",");

        int counter = 0;
        while(token != NULL) {

            if(!counter) {strcpy(word, token); counter++;}
            else {
                prob = atof(token);
                insertMapPrev(mp, prev, word, prob);
                counter = 0;
            }
            token = strtok(NULL, ",");
            
        }
        

    }

    free(prev);
    free(word);
    fclose(csvFile);

}


void produceText(MapPrev *mp, int nWords, char * prevWord) {

    char * word = malloc(30);
    FILE *output;

    if((output = fopen("output.txt", "w+")) == NULL) {
        perror("Error:");
        exit(EXIT_FAILURE);
    }

    if(prevWord == NULL) word = strdup(pickRandomSeparator());
    else {
        toLowerString(prevWord);
        word = strdup(prevWord);
    }
    
    if(!searchMapPrev(mp, word)) {
        printf("Word entered not found: %s\n", word);
        exit(EXIT_FAILURE);
    }

    srand(time(0));

    while(nWords--) {

        unsigned int index = hash(word, mp->nBuckets);
        PrevDictionary *currPrevDict = mp->buckets[index];

        while(currPrevDict != NULL) {
    
            if(strcmp(currPrevDict->word, word) == 0) {
                

                MapFrequency *currMapFreq = currPrevDict->frequencyDict;
                char **words = malloc((currMapFreq->size) * sizeof(char *));
                double *probs = malloc((currMapFreq->size) * sizeof(double));


                int counter = 0;
                for(int i = 0; i < currMapFreq->nBuckets; i++) {

                    Dictionary * currDict = currMapFreq->buckets[i];

                    while(currDict != NULL) {
                        
                        words[counter] = currDict->word;
                        probs[counter] = currDict->frequency;

                        counter++;
                        currDict = currDict->next;

                    }

                }

                int nextWordIndex = randomIndexGenerator(probs, counter);
                char * nextWord = words[nextWordIndex];
                toLowerString(nextWord);

                if(strcmp(word, ".") == 0 || strcmp(word, "!") == 0 || strcmp(word, "?") == 0) {
                    strcpy(word, nextWord);
                    nextWord[0] = toupper(nextWord[0]);

                    fprintf(output, "%s ", nextWord);

                } else {
                 strcpy(word, nextWord);
                 fprintf(output, "%s ", nextWord);   
                }

                if((nWords % 20) == 0) fprintf(output, "\n");

                free(probs);
                free(words);
                break;

            }

                
            currPrevDict = currPrevDict->next;

        }
    
    }

    printf("The text created is in the output.txt file!\n");
    fclose(output);
    free(word);

}