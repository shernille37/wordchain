#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <ctype.h>
#include "dict.h"
#include "file.h"
#include "utils.h"

void readFile(MapPrev * mp, char * fileName, FILE * pipe) {

    FILE *fp;

    // Open file
    if((fp = fopen(fileName, "r")) == NULL) {
        perror("File Error");
        exit(0);
    }

    
    wchar_t word[WORD_SIZE];

    int first = 1;
    wchar_t firstWord[WORD_SIZE];

    // Previous String
    wchar_t prev[WORD_SIZE];

    int i = 0;
    wint_t c;

    int state = 1;

    while((c = fgetwc(fp) ) != EOF) {
        c = (wchar_t) c;

        if(state == 1) STATE_1(&state,word, &i, c, mp, prev, firstWord, &first, pipe);
        else if(state == 2) STATE_2(&state,word, &i, c, mp, prev, firstWord, &first, pipe);
        else STATE_3(&state,word, &i, c, mp, prev, firstWord, &first, pipe);

    }

    // Conditions to handle the word after reaching EOF signal
    if(state == 1) {
        if(pipe) fwprintf(pipe, L"%ls\n", firstWord);
        else insertMapPrev(mp, word, firstWord, -1);
    }
    else if (state == 2) {
        if(pipe) fwprintf(pipe, L"%ls\n", firstWord);
        else insertMapPrev(mp, word, firstWord, -1);
    }
    else if(state == 3) {
        word[i] = '\0';
        toLowerString(word);

        if(pipe) {
            fwprintf(pipe, L"%ls\n", word);
            fwprintf(pipe, L"%ls\n", firstWord);
        } else {
            insertMapPrev(mp, prev, word , -1);
            insertMapPrev(mp, word, firstWord, -1);
        }
    }

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
            fwprintf(fout, L"%ls,", currPrevDict->word);

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

            //Print one line
            int countMapFreq = 0;
            for(int j = 0; j < currMapFreq->nBuckets; j++) {

                Dictionary * currFreqDict = currMapFreq->buckets[j];

                while(currFreqDict != NULL) {
                    fwprintf(fout, L"%ls,", currFreqDict->word);

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

    wchar_t * prev;
    wchar_t * word;
    double prob;

    // Check if the filename provided is a CSV file
    if( strcmp( getFileExtension(filename), "csv" ) != 0 ) {
        printf("The filename shoud be a CSV file!\n");
        exit(EXIT_FAILURE);
    }

    if( (csvFile = fopen(filename, "r")) == NULL) {
        perror("Error");
        exit(EXIT_FAILURE);
    }


    if ((prev = malloc(30 * sizeof(wchar_t))) == NULL) {
        perror("Error:");
        exit(EXIT_FAILURE);
    }   
    if ((word = malloc(30 * sizeof(wchar_t))) == NULL) {
        perror("Error:");
        exit(EXIT_FAILURE);
    }


    while(!feof(csvFile)) {

        wchar_t * line = readLine(csvFile);

        wchar_t * pt;
        wchar_t * token = wcstok(line, L",", &pt);

        prev = wcsdup(token);

        token = wcstok(NULL, L",", &pt);

        int counter = 0;
        while(token != NULL) {

           if(!counter) {wcscpy(word, token); counter++;}
            else {

                prob = wcstod(token, NULL);
                insertMapPrev(mp, prev, word, prob);
                counter = 0;
            }
            token = wcstok(NULL, L",", &pt);
 
        }

        free(line);
    }

    free(prev);
    free(word);
    fclose(csvFile);

}


void produceText(MapPrev *mp, int nWords, char * prevWord) {

    FILE *output;

    wchar_t word[30 * sizeof(wchar_t)];
    
    if((output = fopen("output.txt", "w+")) == NULL) {
        perror("File Creation Error:");
        exit(EXIT_FAILURE);
    }

    if(prevWord == NULL) wcsncpy(word, pickRandomSeparator(mp), sizeof(wchar_t));
    else {
        // Convert multibyte to wchar_t
        size_t wordLength = strlen(prevWord) + 1;

        wchar_t * wc = malloc(wordLength * sizeof(wchar_t));
        mbstowcs(wc, prevWord, wordLength);

        toLowerString(wc);
        wcsncpy(word, wc, 30 * sizeof(wchar_t));
        free(wc);

        if(!searchMapPrev(mp, word)) {
        wprintf(L"Word entered not found: %ls\n", word);
        exit(EXIT_FAILURE);
    }
    
    }
    

    srand(time(0));
    while(nWords--) {


        unsigned int index = hash(word, mp->nBuckets);
        PrevDictionary *currPrevDict = mp->buckets[index];

        
        while(currPrevDict != NULL) {
            if(wcsncmp(currPrevDict->word, word, 30 * sizeof(wchar_t)) == 0) {


                MapFrequency *currMapFreq = currPrevDict->frequencyDict;

                wchar_t ** words = (wchar_t **)malloc(currMapFreq->size * sizeof(wchar_t *));
                double * probs = malloc(currMapFreq->size * sizeof(double));
   


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
                
                wchar_t * nextWord = words[nextWordIndex];
                toLowerString(nextWord);
                
                 if (wcsncmp(nextWord, L".", sizeof(wchar_t)) == 0 || wcsncmp(nextWord, L"!", sizeof(wchar_t)) == 0 || wcsncmp(nextWord, L"?", sizeof(wchar_t)) == 0 || wcsrchr(word, '\'') != NULL) {

                    fseek(output, -1, SEEK_END);
                    wcsncpy(word, nextWord, 30 * sizeof(wchar_t));
                    fwprintf(output, L"%ls ", nextWord);

                } else if(wcsncmp(word, L".", sizeof(wchar_t)) == 0 || wcsncmp(word, L"!", sizeof(wchar_t)) == 0 || wcsncmp(word, L"?", sizeof(wchar_t)) == 0) {
                    wcsncpy(word, nextWord, 30 * sizeof(wchar_t));
                    nextWord[0] = towupper(nextWord[0]);

                    fwprintf(output, L"%ls ", nextWord);

                }
                else {
                 wcsncpy(word, nextWord, 30 * sizeof(wchar_t));
                 fwprintf(output, L"%ls ", nextWord);   
                }


                free(words);
                free(probs);
                
                break;

            }

            currPrevDict = currPrevDict->next;

        }
    
    }

    printf("The text created is in the output.txt file!\n");
    fclose(output);

}