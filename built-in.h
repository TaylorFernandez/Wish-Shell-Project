//
// Created by Taylor Fernandez on 2/16/22.
//

#ifndef WISH_BUILT_IN_H
#define WISH_BUILT_IN_H

#define TRUE 1
#define FALSE  0


void cd(char*);
void echo(char**);
int contains(char **, char*);
int containsChar(char *, char);
int where(char **, char*);
int whereChar(char *, char);
int numWordsInArray(char **, char*);


#endif //WISH_BUILT_IN_H
