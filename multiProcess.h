#ifndef MULTIPROCESS_H
#define MULTIPROCESS_H


// Structure of the Multi-Process version of the Project
void multiProcessi(char * filename, int flag, int nWords, char * prevWord);

// Job Functionalities of the Processes in Compito1
void parentCompito1(int pipe1[2], char * fileName);
void firstChildCompito1(int pipe1[2], int pipe2[2], char * fileName);
void secondChildCompito1(int pipe2[2], char * fileName);

// Job Functionalities of the Processes in Compito2
void parentCompito2(int pipe1[2], char * fileName);
void firstChildCompito2(int pipe1[2], int pipe2[2], char * fileName, int nWords, char * prevWord);
void secondChildCompito2(int pipe2[2], char * fileName);

#endif