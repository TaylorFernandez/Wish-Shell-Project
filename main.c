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

/**
 * Global variables used through out main and doCommands functions
 */
char path[10][100] = {"/bin"};
char **array;
void doCommands(char **, int, int, char *);
void *threadExecute(void *);
int parentPID;


/**
 * Main Loop of the program. When used in input mode, the shell will continue to collect and parse user
 * input and perform operations until the user inputs "exit." The shell will only exit if the input is "exit"
 * with no other Arguments. When in batch mode, the program will read in lines of a batch file, parse the input,
 * and perform the necessary operations until the end of the file is reached.
 * @param argc - number of arguments
 * @param argv - pointer to a list of arguments
 * @return execution is sucessful. If wish encounters an error throughout execution, and error will be
 *          printed to Standard Error and will return 1. Errors encountered during input mode will
 *          simply print an error to Standard Error and continue gathering user input.
 */
int main(int argc, char* argv[]) {
    int isExiting = FALSE;
    char inputLine[100] = "\0";
    unsigned long lenBuffer = 0;
    char *buffer = NULL;

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
             * by looking at the first word and checking if the user
             * wants to exit. If not, wish will send parsed commands
             * to doCommands()
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



    /**
     * Parallelization: Handles cases where the user inputs parallel
     * commands separated by "&" without a space between commands
     * EX: "ls&echo test
     */
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

    /**
     * Parallelization: Handles when the user inputs parallel commands separated by "&" or
     * when the commands are separated in the "if" block above. This if block will record
     * each word in the command until it finds the "&" symbol. When found, the current list
     * of strings will be placed in a char *** variable, pointing to a list of list of strings
     *
     * Limitation: Only able to handle 3 commands at most. Can be increased by making
     * listArgs larger and adding more char ** lists.
     */
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


    /**
     * Redirection: This "if" block handles when the user wants to redirect output
     * by using the ">" symbol. The if block will separate any commands where the user
     * doesn't separate the command, ">", and destination with a space. Once the command
     * is separated, a variable will be set to TRUE and the ">" and destination in array[]
     * will become null to support execv properly
     */
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

    /**
     * This if block handles all user commands including "&" and ">". If a command
     * is not a built in command (see README), the program will send the array
     * containing the command and arguments to the else block where
     * running external executables will be possible. Wish will check all
     * path variables and determine if the command is found. If it is, wish will
     * create a child process and run the executable. If not, wish will write an
     * error to Standard Error and continue to the next line.
     */
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
                    /**
                     * if the ">" character is found in the command and the command
                     * is found in one of the possible paths, wish will redirect output
                     * from STDOUT to a file descriptor pointing to the file provided.
                     * If the file is not found, wish will create a new file and output
                     * there. If the file is found, will simply open the file and write to it.
                     */
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
        /**
         * if a build in command is not found, nor is the command an executable
         * found in the possible paths, wish will write and error to STANDARD ERROR.
         */
        if (success == FALSE) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }
}
