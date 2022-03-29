//
// Created by Taylor Fernandez on 2/16/22.
//
#include <string.h>
#include <stdlib.h>
#include "wordFormatting.h"
#include <stdio.h>

/**
 * Splits the string "buffer" into a list of words, separated by a space
 * @param buffer - String input from main or file
 * @return array containing strings
 */
char* parse(char *buffer){
    int i = 0;
    char *split = strtok(buffer, " ><\t\r\f");
    char **array = malloc(15 * sizeof(char *));
    /**
     * uses the strtok function to parse the string into
     * a list of words
     */
    while (split != NULL) {
        array[i++] = split;
        split = strtok(NULL, " \t\r\f\n");
    }
    if(array[i-1][strlen(array[i-1]) - 1] == '\n') {
        array[i - 1][strlen(array[i - 1]) - 1] = '\0';
    }

    return array;
}

char *parseNew(char *str, char *delim){
    int i = 0;
    char *first = strtok(str, delim);
    char **arr = malloc(5 * sizeof(char *));

    while(first != NULL){
        arr[i] = first;
        first = strtok(NULL, delim);
        i++;
    }

    return arr;
}
/**
 * Counts the different words
 * @param buffer - string input from the file or shell
 * @return number of words
 */
int numWords(char **array){
    int i = 0;
    while(array[i] != NULL){
        i++;
    }
    return i;
}