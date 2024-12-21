#ifndef SOLVER_H
#define SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @args
 * - `file_name` : char[] of the name of the file
 * - `word` : char[] of the word you search in the hidden words
 * @return
 * - Nothing it print the anwser
 */
void Solver(char file_name[], char word[],int *X_prem, int *Y_prem, int *X_der, int *Y_der);



/**
 * @args
 * - 'tableau' : pointer to a 2d array
 * - 'j' : int that give y direction
 * - 'i' : int that give x direction
 * - 'word' : char[] of the word you search in the hidden words
 * - 'p' : int of the y position in the array
 * - 'l' : int of the x position in the array
 * - 'len' : size of the word
 * @return
 * - int 0 or 1 that work as a bool
 * - 1 if the word is in this direction
 * - 0 else
 */
int findings(char** tableau,int j,int i,char word[],int p,int l,size_t len);


#endif /* ! SOLVER_H */