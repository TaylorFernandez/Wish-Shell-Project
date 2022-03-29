//
// Created by Taylor Fernandez on 2/16/22.
//

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "built-in.h"
#include <stdio.h>

#include "wordFormatting.h"


/**
 * used to clear the terminal
 */
void clear(){
    system("clear");
}

/**
 * sets exiting to true
 * @return TRUE
 */
int exiting(){
    return TRUE;
}
/**
 * CDs into a new path, given by buffer. Handles if the last character in the path is a newline character
 * @param buffer - path
 */
void cd(char *buffer){
    chdir(buffer);
}

int contains(char **str, char *letter){
    int len = numWords(str);

    for(int i = 0; i < len; i++){
        if(strcmp(str[i], letter) == 0){
            return TRUE;
        }
    }
    return FALSE;
}

int containsChar(char *str, char letter){
    int len = strlen(str);

    for(int i = 0; i < len; i++){
        if(str[i] == letter){
            return TRUE;
        }
    }
    return FALSE;
}

int where(char **str, char *letter){
    int len = numWords(str);

    for(int i = 0 ; i < len; i ++){
        if(strcmp(str[i], letter) == 0){
            return i;
        }
    }
    return -1;
}

int whereChar(char *str, char letter){
    int len = strlen(str);

    for(int i = 0 ; i < len; i ++){
        if(str[i] == letter){
            return i;
        }
    }
    return -1;
}

int numWordsInArray(char **array, char *word){
    int len = numWords(array);
    int count = 0;
    for(int i = 0; i < len; i++){
        if(strcmp(array[i], word) == 0){
            count++;
        }
    }

    return count;
}