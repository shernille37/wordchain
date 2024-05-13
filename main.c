#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include "dict.h"
#include "file.h"


int main(int argc, char *argv[]) {

    setlocale(LC_ALL, "C.UTF-8");
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

    if(compito1Flag && compito2Flag) {
        printf("Only 1 option is required: -c or -r\n");
        exit(EXIT_FAILURE);
    }

    if(filename == NULL) {
        printf("You have to provide for a filename: -f <filename>\n");
        exit(EXIT_FAILURE);
    }


    // Initialize HashMap
    MapPrev *mp = initMapPrev();

    if(compito1Flag) {
        // Compito 1
        readFile(mp, filename);
        //printMapPrev(mp);
        writeCsv(mp, filename);

    } else if(compito2Flag) {

        // Compito 2
        readCsv(mp, filename);
        //printMapPrev(mp);
        produceText(mp, nWords, prevWord);
    }

        
    freeMapPrev(mp);
    
    clock_t end = clock();

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time: %f ms\n", time_spent * 1000);

    // Compito 2

    return 0;
}
