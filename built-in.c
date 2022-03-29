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

/**
 * contains checks to see if an array contains a certain string
 * @param str - list of strings split by parse()
 * @param letter - string that will be checked in str
 * @return - TRUE if letter is found in string: FALSE if letter is not found in string
 */
int contains(char **str, char *letter){
    int len = numWords(str);

    for(int i = 0; i < len; i++){
        if(strcmp(str[i], letter) == 0){
            return TRUE;
        }
    }
    return FALSE;
}
/**
 * ContainsChar is similar to contains() but checks a string not parsed by parse()
 * to check if a letter is in a string.
 * @param str - string
 * @param letter - letter to be checked for
 * @return - TRUE if letter is found in string: FALSE if letter is not found in string
 */
int containsChar(char *str, char letter){
    int len = strlen(str);

    for(int i = 0; i < len; i++){
        if(str[i] == letter){
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * Where() finds the first occurance of a string in a list of strings
 * @param str - list of strings
 * @param letter - string to be found
 * @return first occurance of the string in the list: 0-indexed
 */
int where(char **str, char *letter){
    int len = numWords(str);

    for(int i = 0 ; i < len; i ++){
        if(strcmp(str[i], letter) == 0){
            return i;
        }
    }
    return -1;
}

/**
 * Where() finds the first occurance of a letter in a string
 * @param str - String
 * @param letter - letter to be found
 * @return first occurance of the letter in the string: 0-indexed
 */
int whereChar(char *str, char letter){
    int len = strlen(str);

    for(int i = 0 ; i < len; i ++){
        if(str[i] == letter){
            return i;
        }
    }
    return -1;
}

/**
 * Finds the number of occurances of a word in a list of words
 * @param array - list of words
 * @param word - word to be found
 * @return number of times word is found in array
 */
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