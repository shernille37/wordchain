#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include "dict.h"
#include "file.h"
#include "multiProcess.h"


int main(int argc, char *argv[]) {

    setlocale(LC_ALL, "en_US.UTF-8");
    clock_t begin = clock();

    int opt;
    int uniProcessFlag = 0;
    int multiProcessFlag = 0;
    int compito1Flag = 0;
    int compito2Flag = 0;
    char * filename = NULL;

    int nWords = 0;
    char * prevWord = NULL;
    

      while( (opt = getopt(argc, argv, "umcrn:p:f:")) != -1) {

        switch (opt)
        {
        case 'u':
            uniProcessFlag = 1;
            break;
        
        case 'm':
            multiProcessFlag = 1;
            break;
        
        case 'f':
            filename = optarg;
            break;
        
        case 'c':
            compito1Flag = 1;
            break;
        
        case 'r':
            compito2Flag = 1;
            break;
        
        case 'n':
            nWords = atoi(optarg);
            break;
        
        case 'p':
            prevWord = optarg;
            break;
            
        
        case '?':
            if(optopt == 'f')
                fprintf(stderr, "Option -%c requires an argument <filename>\n", optopt);
            else if(optopt == 'n')
                fprintf(stderr, "Option -%c requires an argument <number of words>\n", optopt);    
            else if(optopt == 'p')
                fprintf(stderr, "Option -%c requires an argument <Previous Word>\n", optopt);
            else if(isprint(optopt)) fprintf(stderr, "Unknown option -%c\n", optopt);
            
            exit(EXIT_FAILURE);
        
        default:
            break;
        }
    };

    if(uniProcessFlag && multiProcessFlag){
        printf("Only 1 option is required: -u or -m\n");
        exit(EXIT_FAILURE);
    }

    if(!uniProcessFlag && !multiProcessFlag){
        printf("Atleast 1 option is required: -u or -m\n");
        exit(EXIT_FAILURE);
    }

    if(compito1Flag && compito2Flag) {
        printf("Only 1 option is required: -c or -r\n");
        exit(EXIT_FAILURE);
    }

    if(!compito1Flag && !compito2Flag) {
        printf("Atleast 1 option is required: -c or -r\n");
        exit(EXIT_FAILURE);
    }

    if(filename == NULL) {
        printf("You have to provide for a valid filename: -f <filename>\n");
        exit(EXIT_FAILURE);
    }

    


    if(uniProcessFlag) {

        // Initialize Data Structure (HashMap)
        MapPrev *mp = initMapPrev();
        if(compito1Flag) {
            readFile(mp, filename, NULL);
            //printMapPrev(mp);
            writeCsv(mp, filename, NULL);

        } else if(compito2Flag) {

            readCsv(mp, filename);
            //printMapPrev(mp);
            produceText(mp, nWords, prevWord);
        }

        freeMapPrev(mp);

    } else {

        if(compito1Flag) {
            multiCompito1(filename);
        }
   
    }

        
    
    clock_t end = clock();

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time: %f ms\n", time_spent * 1000);


    return 0;
}
