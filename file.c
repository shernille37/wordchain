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


/*
Function to read the file in input
and populate the MapPrev


Input:
-- Instance of MapPrev
-- Filename provided via command line
-- Eventual PIPE FILE (Multi-process version)

Multi-Process Version:
Instead of populating the MapPrev
it sends the words into the pipe

*/
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

    /*
    Read the file character by character
    and change state based on the character read

    Finite State Machine

    STATE 1: Punctiations (! . ?)
    STATE 2: Separators (any characters that is not a standard character)
    STATE 3: Letter/Character

    */
    while((c = fgetwc(fp) ) != EOF) {
        c = (wchar_t) c;

        if(state == 1) STATE_1(&state,word, &i, c, mp, prev, firstWord, &first, pipe);
        else if(state == 2) STATE_2(&state,word, &i, c, mp, prev, firstWord, &first, pipe);
        else STATE_3(&state,word, &i, c, mp, prev, firstWord, &first, pipe);

    }

    // Conditions to handle the word after reaching EOF signal
    if(state == 1 || state == 2) {
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

/*
Function to write the CSV file as output
reading the entries in the MapPrev

Input:
-- MapPrev instance
-- Filename provided via command line
-- Eventual PIPE FILE (multi-process version)

Multi-Process Version:
Instead of printing it to the output file
It sends it to the pipe for processing

*/

void writeCsv(MapPrev * mp, char * filename, FILE * pipe) {

    FILE *fout;

    // Change the file extension of the filename to a .csv file for the output
    char * newFileName = changeFileExtension(filename, "csv");

    if( (fout = fopen(newFileName, "w+")) == NULL ) {
        perror("Error:");
        exit(EXIT_FAILURE);
    }


    // Iterate through the entries of the MapPrev
    int countMapPrev = 0;
    for(int i = 0; i < mp->nBuckets; i++) {

        PrevDictionary * currPrevDict = mp->buckets[i];

        while(currPrevDict != NULL) {
            
            // If pipe is provided then send it to the pipe otherwise directly print it into the output file
            if(pipe) fwprintf(pipe, L"%ls,\n", currPrevDict->word);
            else fwprintf(fout, L"%ls,", currPrevDict->word);
            

            // Compute Sum
            double sum = 0;
            char n[20];

            MapFrequency * currMapFreq = currPrevDict->frequencyDict;

            // Calculate sum of the frequencies
            for(int j = 0; j < currMapFreq->nBuckets; j++) {   

                Dictionary * currFreqDict = currMapFreq->buckets[j];

                while(currFreqDict != NULL) {
                    sum += currFreqDict->frequency;
                    currFreqDict = currFreqDict->next;
                }
            }

            //Print one line of the csv file
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
    
    printf("CSV file generated: %s\n", newFileName);
    free(newFileName);
    fclose(fout);

}

/*
Function to read the CSV file
Populate the MapPrev

Input:
-- Instance of MapPrev
-- Filename provided via command line
-- Eventual PIPE FILE (Multi-Process version)

Multi-Process Version:
Instead of populating MapPrev
it sends it to the PIPE for processing

*/

void readCsv(MapPrev *mp, char * filename, FILE * pipe) {

    FILE * csvFile;

    // Check if the filename provided is a CSV file
    if( strncmp( getFileExtension(filename), "csv", WORD_SIZE ) != 0 ) {
        printf("The filename shoud be a CSV file!\n");
        exit(EXIT_FAILURE);
    }

    if( (csvFile = fopen(filename, "r")) == NULL) {
        perror("Error");
        exit(EXIT_FAILURE);
    }


    wchar_t prev[WORD_SIZE];
    wchar_t word[WORD_SIZE];
    wchar_t nextWord[WORD_SIZE];
    double prob;

    //Read the CSV file line by line until it reaches EOF
    while(!feof(csvFile)) {

        // Read one line of the CSV file
        wchar_t * line = readLine(csvFile);

        wchar_t * pt;
        wchar_t * token = wcstok(line, L",", &pt); // Split the line with ',' as delimeters to obtain the words and probabilities

        // If there's a pipe: Send the previous word into the pipe
        if(pipe) fwprintf(pipe, L"%ls\n", token);

        wcsncpy(prev, token, WORD_SIZE);

        // Next token
        token = wcstok(NULL, L",", &pt);

        /*
        Iterate through the eventual tokens and use a binary counter
        to distinguish tokens between a word or a probability (Number)

        counter 0: Word
        counter 1: Probability (Number)
        */
        int counter = 0;
        while(token != NULL) {
           if(!counter) {wcsncpy(word, token, WORD_SIZE); counter++;}
            else {

                prob = wcstod(token, NULL);
                
                // Send the next word and the probability
                if(pipe) {
                    fwprintf(pipe, L"%ls\n", word);
                    fwprintf(pipe, L"%lf\n", prob);
                } else insertMapPrev(mp, prev, word, prob);
                
                counter = 0;
            }
            token = wcstok(NULL, L",", &pt);
 
        }

        // Send -1 into the pipe to indicate that it reached the end of the line
        if(pipe) fwprintf(pipe, L"%ls\n", L"-1");

        free(line);
    }

    fclose(csvFile);

}

/*
Generate random text using entries in the MapPrev

Input:
-- Instance of MapPrev
-- Number of words provided via command line
-- Eventual Previous Word provided via command line
-- Eventual PIPE FILE  (Multi-Process version)

Multi-Process Version:
Instead of printing the word generated in the output file
send it to the pipe for processing and printing

*/

void produceText(MapPrev *mp, int nWords, char * prevWord, FILE * pipe) {

    // Random Number Generator using GNU Scientific Library

    // Initialize the random number generator
    gsl_rng_env_setup();
    gsl_rng* r = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(r, time(NULL)); // Seed the random number generator


    FILE * output;
    if((output = fopen("output.txt", "w+")) == NULL) {
        perror("File Creation Error:");
        exit(EXIT_FAILURE);
    }
    
    wchar_t word[WORD_SIZE];

    // If the previous word is provided, pick a random separator
    if(prevWord == NULL) wcsncpy(word, pickRandomSeparator(mp, r), sizeof(wchar_t));
    else {
        // Convert multibyte to wchar_t
        size_t wordLength = strlen(prevWord) + 1;

        wchar_t * wc = malloc(wordLength * sizeof(wchar_t));
        mbstowcs(wc, prevWord, wordLength);

        toLowerString(wc);
        wcsncpy(word, wc, WORD_SIZE);
        free(wc);

        // Check if the previous word provided is present in the file, otherwise pick a random separator
        if(!searchMapPrev(mp, word)) {
            wprintf(L"Word entered not found: %ls\n", word);
            wcsncpy(word, pickRandomSeparator(mp, r), sizeof(wchar_t));
        }
    
    }

    // Iterate until the number of words reaches 0
    while(nWords--) {

        // Get the index of the word in the HashMap
        unsigned int index = hash(word, mp->nBuckets);
        PrevDictionary *currPrevDict = mp->buckets[index];

        // Iterate through the Linked List
        while(currPrevDict != NULL) {
            if(wcsncmp(currPrevDict->word, word, WORD_SIZE) == 0) {


                MapFrequency *currMapFreq = currPrevDict->frequencyDict;
                
                /*
                    Dynamically allocate arrays to store the words and the probabilities
                */
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

                // Get the discrete random numbers 
                int nextWordIndex = gsl_ran_discrete(r, dist);

                // Get the next word from the Words array using the nextWordIndex generated randomly
                wchar_t * nextWord = words[nextWordIndex];
                toLowerString(nextWord);

                
                /*
                    Logic for printing:
                    -- If the next word is one of the puntuations(. ! ?) OR word containing an apostrophe
                    the next word printed will be attached to the previous word
                    -- If the previous word is a punctuation: then the next word printed will have an UPPERCASE initial
                
                */


                if (wcsncmp(nextWord, L".", sizeof(wchar_t)) == 0 || wcsncmp(nextWord, L"!", sizeof(wchar_t)) == 0 || wcsncmp(nextWord, L"?", sizeof(wchar_t)) == 0 || wcsrchr(word, L'\'') != NULL || wcsrchr(word, L'’') != NULL) {

                    wcsncpy(word, nextWord, WORD_SIZE);

                    if(pipe) {
                        fwprintf(pipe, L"%ls\n", L"-1");
                        fwprintf(pipe, L"%ls \n", nextWord);                        
                    } else {
                        fseek(output, -1, SEEK_END);
                        fwprintf(output, L"%ls ", nextWord);
                    }


                } else if(wcsncmp(word, L".", sizeof(wchar_t)) == 0 || wcsncmp(word, L"!", sizeof(wchar_t)) == 0 || wcsncmp(word, L"?", sizeof(wchar_t)) == 0) {
                    wcsncpy(word, nextWord, WORD_SIZE);
                    nextWord[0] = towupper(nextWord[0]);

                    if(pipe) fwprintf(pipe, L"%ls \n", nextWord);
                    else fwprintf(output, L"%ls ", nextWord);

                }
                else {
                wcsncpy(word, nextWord, WORD_SIZE);

                if(pipe) fwprintf(pipe, L"%ls \n", nextWord);
                else fwprintf(output, L"%ls ", nextWord);   
                }

                free(words);
                free(probs);

                // Free the discrete distribution
                gsl_ran_discrete_free(dist);

                break; // Proceed to the next word

            }

            currPrevDict = currPrevDict->next;

        }
    
    }

    /*
        The generated words will be in the output.txt file
    */
    printf("The text created is in the \"output.txt\" file!\n");

    gsl_rng_free(r);
    fclose(output);

}