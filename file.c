#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

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

void writeCsv(MapPrev * mp, char * filename, FILE * pipe) {

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


            if(pipe) fwprintf(pipe, L"%ls,\n", currPrevDict->word);
            else fwprintf(fout, L"%ls,", currPrevDict->word);
            

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



                    if(pipe) fwprintf(pipe, L"%ls,\n", currFreqDict->word);
                    else fwprintf(fout, L"%ls,", currFreqDict->word);

                    double prob = currFreqDict->frequency/sum;
                    //Convert the probability into a string to easily modify its format
                    sprintf(n, "%f", prob);

                    // Remove trailing zeros while maintaining the significant digits
                    removeZeros(n);

                    if(pipe) fprintf(pipe, "%s\n", n);
                    else fprintf(fout, "%s", n);

                       
                    countMapFreq++;

                    if(countMapFreq < currMapFreq->size) {
                        if(pipe) fprintf(pipe, "%s\n", ",");
                        else fprintf(fout, "%s", ",");
                        
                    }
                    currFreqDict = currFreqDict->next;
                    
                }
            }


            countMapPrev++;
            if(countMapPrev < mp->size) {
                if(pipe) fprintf(pipe, "%s\n", "-1");
                else fprintf(fout, "%s", "\n"); 
            }
            
            currPrevDict = currPrevDict->next;

        }

    }
    
    
    free(newFileName);
    fclose(fout);

}


void readCsv(MapPrev *mp, char * filename) {

    FILE *csvFile;

    wchar_t prev[WORD_SIZE];
    wchar_t word[WORD_SIZE];
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

    while(!feof(csvFile)) {

        wchar_t * line = readLine(csvFile);

        wchar_t * pt;
        wchar_t * token = wcstok(line, L",", &pt);

        wcsncpy(prev, token, WORD_SIZE);

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

    fclose(csvFile);

}


void produceText(MapPrev *mp, int nWords, char * prevWord) {

    // Initialize the random number generator
    gsl_rng_env_setup();
    gsl_rng* r = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(r, time(NULL)); // Seed the random number generator


    FILE *output;
    wchar_t word[WORD_SIZE];

    if((output = fopen("output.txt", "w+")) == NULL) {
        perror("File Creation Error:");
        exit(EXIT_FAILURE);
    }

    if(prevWord == NULL) wcsncpy(word, pickRandomSeparator(mp, r), sizeof(wchar_t));
    else {
        // Convert multibyte to wchar_t
        size_t wordLength = strlen(prevWord) + 1;

        wchar_t * wc = malloc(wordLength * sizeof(wchar_t));
        mbstowcs(wc, prevWord, wordLength);

        toLowerString(wc);
        wcsncpy(word, wc, WORD_SIZE);
        free(wc);

        if(!searchMapPrev(mp, word)) {
            wprintf(L"Word entered not found: %ls\n", word);
            exit(EXIT_FAILURE);
        }
    
    }


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

                // Create a GSL discrete distribution from the probabilities
                gsl_ran_discrete_t* dist = gsl_ran_discrete_preproc(currMapFreq->size, probs);

                int nextWordIndex = gsl_ran_discrete(r, dist);

                wchar_t * nextWord = words[nextWordIndex];
                toLowerString(nextWord);
                
                 if (wcsncmp(nextWord, L".", sizeof(wchar_t)) == 0 || wcsncmp(nextWord, L"!", sizeof(wchar_t)) == 0 || wcsncmp(nextWord, L"?", sizeof(wchar_t)) == 0 || wcsrchr(word, '\'') != NULL) {

                    fseek(output, -1, SEEK_END);
                    wcsncpy(word, nextWord, WORD_SIZE);
                    fwprintf(output, L"%ls ", nextWord);

                } else if(wcsncmp(word, L".", sizeof(wchar_t)) == 0 || wcsncmp(word, L"!", sizeof(wchar_t)) == 0 || wcsncmp(word, L"?", sizeof(wchar_t)) == 0) {
                    wcsncpy(word, nextWord, WORD_SIZE);
                    nextWord[0] = towupper(nextWord[0]);

                    fwprintf(output, L"%ls ", nextWord);

                }
                else {
                 wcsncpy(word, nextWord, WORD_SIZE);
                 fwprintf(output, L"%ls ", nextWord);   
                }


                free(words);
                free(probs);
                
                gsl_ran_discrete_free(dist);
                break;

            }

            currPrevDict = currPrevDict->next;

        }
    
    }

    printf("The text created is in the output.txt file!\n");

    gsl_rng_free(r);
    fclose(output);

}