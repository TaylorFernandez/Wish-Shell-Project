#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/fcntl.h>
#include "built-in.h"
#include "wordFormatting.h"
#include <pthread.h>

//Next Task:
//  Use #include <unistd.h> and getcwd() and chdir() for cd commands (Done)
//

/**
 * !!!Change path in execv to PATH, not hardcode
 */
/**
 * This program acts as a shell program. It takes in user strings
 * does certain things based on the input
 * @param argc - number of arguments
 * @param argv - list of the arguments
 */

typedef struct{
    char **args;
    int num;
    int bool;
} holder;

char path[10][100] = {"/bin"};
char **array;
void doCommands(char **, int, int, char *);
void *threadExecute(void *);
int parentPID;

int main(int argc, char* argv[]) {
    int isExiting = FALSE;
    char inputLine[100] = "\0";
    unsigned long lenBuffer = 0;
    char *buffer = NULL;

    /**
     * PATH: create a list that holds a path. Use a while loop until null to store the paths in an array.
     * When PATH is called again, clear the list and overwrite it. When path is called with no parameters,
     * (index PATH starting at 1
     */

    /**
     * If there are more than 1 arguments, the program will attempt to
     * run a batch file. If there is only one, the program
     * will enter its main shell stage
     */
    if (argc > 1) {
        FILE *inF = fopen(argv[1], "r");
        if(inF == NULL){
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return 1;
        }
        int temp = getline(&buffer, &lenBuffer, inF);
        if(temp == -1 ){
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return 1;
        }
        while (temp != -1) {
            char tempLn[100];
            strcpy(tempLn, buffer);
            char **array = parse(buffer);
            int numberOfWords = numWords(array);


            if(strcmp(array[0], "exit") == 0){
                if(numberOfWords != 1){
                    char error_message[30] = "An error has occurred\n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }else {
                    //Closes the file to avoid memory leaks
                    fclose(inF);

                    return 0;
                }
            }else{
                doCommands(array, numberOfWords, FALSE, tempLn);
            }
            temp = getline(&buffer, &lenBuffer, inF);
        }
    }else {
        /**
         * This loop acts as the body of the program. It will constantly
         * accept and parse user input until the user signals they want
         * to exit
         */
        while (isExiting == FALSE) {
            char cwd[100] = "\0";
            getcwd(cwd, 100);
            printf("wish>");
            scanf("%[^\n]%*c", inputLine);
            char temp[100];
            strcpy(temp, inputLine);
            char **array = parse(inputLine);
            int counting = numWords(array);
            /**
             * this portion of the function handles user commands
             * by looking at the first word in the list and
             * checking if its a valid command
             */
            if( strcmp(array[0], "exit") == 0){
                if(counting != 1){
                    char error_message[30] = "An error has occurred\n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }else {
                    return 0;
                }
            }else{
                doCommands(array, counting, FALSE, temp);
            }
        }
    }

}
/**
 * This function handles each line input. It uses the array of words and does the operation requested, then returns to the main function
 * @param array - List of words without space characters
 * @param numberOfWords - number of words in the list
 * @param isExec - Whether or not the instructions are coming from a file/script
 */
void doCommands(char **array, int numberOfWords, int isExec, char *input) {

    int pass = TRUE;
    int redirect = FALSE;
    char source[100];
    char destination[100];
    int parallel = FALSE;

    //Need To Do: Parallel Commands
    //Splitting strings working: Need to fix seconds command not splitting correctly without proper spacing
    //Ex: ls&echo test


    if(containsChar(array[0], '&') == 1 && contains(array, "&") != TRUE){
        char and[3] = "&";
        char **words = parseNew(input, "&");
        int i = 0;
        int j = 0;
        while(words[j] != NULL){
            if(containsChar(words[j], ' ')==TRUE){
                int f = 0;
                char **temp = parseNew(words[j], " ");
                while(temp[f] != NULL){
                    array[i] = temp[f];
                    i++;
                    f++;
                }
            }else {
                array[i] = words[j];
            }
            if(words[j+1] != NULL) {
                i++;
                array[i] = and;
            }
            i++;
            j++;
        }
        i = 0;
    }

    //p1.sh&p2.sh&p3.sh and p1.sh & p2.sh & p3.sh are the same when entering this if block
    if (contains(array, "&") == TRUE && strcmp(array[0], "&") != 0) {

        pthread_t test[3];
        parallel = TRUE;
        char **args1 = malloc(5 * sizeof(char *));
        char **args2 = malloc(5 * sizeof(char *));
        char **args3 = malloc(5 * sizeof(char *));
        char ***listArgs = malloc(5 * sizeof(char **));

        listArgs[0] = args1;
        listArgs[1] = args2;
        listArgs[2] = args3;
        int k = 0;
        int i = 0;
        int j = 0;
        while (array[i] != NULL) {
            if (strcmp(array[i], "&") != 0) {
                char *temp = array[i];
                listArgs[k][j] = temp;
                i++;
                j++;
            } else {
                i++;
                j = 0;
                k++;
            }

        }

        int tempNumForks = 0;
        for(int t = 0; t < 3; t++)
        {
            if(listArgs[t][0] != NULL){
                tempNumForks++;
            }
        }

        pid_t forks[3];

//        for(int t = 0; t < 3; t++){
//            for(int y = 0; y < 5; y++){
//                printf("%s ", listArgs[t][y]);
//            }
//            printf("\n");
//        }

        for (int t = 0; t < tempNumForks; t++) {
            if(fork() == 0){
                doCommands(listArgs[t], numberOfWords, TRUE, NULL);
                forks[t] = getpid();
                kill(getpid(), SIGKILL);
            }


        }
        for(int t = 0; t < tempNumForks; t++){
            wait(&forks[t]);
        }

    }



    if (array[1] != NULL && containsChar(array[1], '>') == TRUE && whereChar(array[1], '>') != strlen(array[1]) &&
        contains(array, ">") == FALSE) {
        int loc = whereChar(array[1], '>');
        int i = 0;
        if (loc == 0) {
            while (array[1][i + 1] != '\0') {
                destination[i] = array[1][i + 1];
                i++;
            }
            array[1] = NULL;
            redirect = TRUE;
        } else {
            while (array[1][i] != '>') {
                source[i] = array[1][i];
                i++;
            }
            source[i++] = '\0';
            int j = 0;
            while (array[1][i] != '\0') {
                destination[j] = array[1][i];
                i++;
                j++;
            }
            destination[j++] = '\0';
            redirect = TRUE;
            array[1] = NULL;
            array[1] = source;
        }
    } else if(contains(array, ">") == TRUE && array[1] != NULL && strcmp(array[1], ">") == 0 && array[2] != NULL && array[3] == NULL) {
        redirect = TRUE;
        strcpy(destination, array[2]);
        array[1] = NULL;
        array[2] = NULL;
    }else if (contains(array, ">") == TRUE && (array[2] == NULL || array[3] != NULL)) {
        pass = FALSE;
    }

    if (parallel == TRUE) {
        
    } else if (pass == FALSE) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
    } else if (array[0][0] == '#') {


    } else if (strcmp(array[0], "&") == 0) {

    } else if ((strcmp(array[0], "cd") == 0)) {
        if (numberOfWords != 2) {

            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        } else {
            chdir(array[1]);
        }
    } else if (strcmp(array[0], "cat") == 0 && numberOfWords == 1) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
    } else if (strcmp(array[0], " ") == 0 || strcmp(array[0], "\t") == 0) {

    } else if (strcmp(array[0], "path") == 0 || strcmp(array[0], "path\n") == 0) {
        if (array[1] == NULL) {
            for (int i = 0; i < 10; i++) {
                strcpy(path[i], "\0");
            }
        } else {
            int i = 0;
            while (array[i + 1] != NULL) {
                strcpy(path[i], array[i + 1]);
                i++;
            }
        }
    } else {
        int i = 0;
        int pid = 0;
        int success = FALSE;
        while (path[i] != NULL && i < 10) {
            char temp[256];
            strcpy(temp, path[i]);
            strcat(temp, "/");
            if(array[0][strlen(array[0]) - 1] == '\n'){
                array[0][strlen(array[0]) - 1] = '\0';
            }
            strcat(temp, array[0]);

            if (access(temp, X_OK) == 0) {

                if (fork() == 0) {
                    if (redirect == TRUE) {
                        int fd = open(destination, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                        dup2(fd, 1);

                        close(fd);
                    }
                    pid = getpid();
                    execv(temp, array);
                    kill(pid, SIGKILL);
                } else {
                    wait(&pid);
                    success = TRUE;
                    break;
                }
            }
            i++;
        }

        if (success == FALSE) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }
}
