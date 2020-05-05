#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "shellmemory.h"

int parse(char input[])
{

    char * parsedInput[4] = {NULL, NULL, NULL, NULL};
    int numTokens = 0;
    char *ptr = strtok(input, " "); //tokenize by " " character
    while(ptr != NULL){
        numTokens++;
        if(numTokens > 4){
            printf("Invalid command, maximum number of arguments allowed is 4\n");
            return 0;
        }
        parsedInput[numTokens-1] = ptr;
        ptr = strtok(NULL, " ");
    }
    
    return interpreter(parsedInput, numTokens);
}

int shellUI()
{

    int errorCode = 0;
    puts("Welcome to the Nicolas Buisson shell!\nShell version 2.0 Updated February 2020");
    printf("Kernel 1.0 loaded!\n");
    initializeMemory();
    char *str = (char *)(malloc(sizeof(char) * 1000));
    while (errorCode != -1)
    {
        printf("$ ");
        fgets(str, 1000, stdin);
        if(strcmp(str, "\n")==0){
            printf("Must enter a command\n");
            continue;
        }
        //filter out '\n' characters from input
        int length = strlen(str);
        if(*(str + length - 1) == '\n'){
            *(str + length -1) = '\0';
        }
        errorCode = parse(str);
    }
}