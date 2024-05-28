#ifndef MULTIPROCESS_H
#define MULTIPROCESS_H

void multiProcessi(char * filename, int flag);

void parentCompito1(int pipe1[2], char * fileName);
void firstChildCompito1(int pipe1[2], int pipe2[2], char * fileName);
void secondChildCompito1(int pipe2[2], char * fileName);

#endif