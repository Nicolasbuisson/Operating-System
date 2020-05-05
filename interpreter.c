#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "shellmemory.h"
#include "pcb.h"
#include "ram.h"
#include "kernel.h"
#include "memorymanager.h"

int help()
{
    //prints all of the shell's functions and function descriptions
    printf("help:                                           Displays all the commands and their descriptions\n");
    printf("quit:                                           Exits the program\n");
    printf("set 'VAR' 'STRING':                             Assigns a STRING value to VAR in the shell memory\n");
    printf("print 'VAR':                                    Displays the STRING assigned to VAR\n");
    printf("run 'SCRIPT.TXT':                               Executes the file SCRIPT.TXT\n");
    printf("exec 'SCRIPT1.TXT' 'SCRIPT2.TXT' 'SCRIPT3.TXT': Executes concurrent programs\n");
    return 0;
}

int quit()
{
    printf("Bye!\n");
    return -1;
    // exits the program
}

int set(char *words[])
{
    write(words[1], words[2]);
    return 0;
}

int print(char *words[])
{
    read(words[1]);
    return 0;
}

static int run(char *words)
{
    int errCode = 0;
    char line[1000];
    FILE *p;
    p = fopen(words, "rt");
    if (p == NULL)
    {
        printf("Could not open the selected file: %s\n", words);
        return 0;
    }

    while (!feof(p))
    {
        char *line = malloc(1000 * (sizeof(char)));
        fgets(line, 1000, p);
        //filter out '\n' characters from the input
        int length = strlen(line);
        if (*(line + length - 1) == '\n')
        {
            *(line + length - 1) = '\0';
        }
        if(strcmp(line, "quit") == 0){
            break;
        }
        parse(line);
        memset(line, ' ', 1000 * sizeof(char));
    }
    return 0;
}

int exec(char *words[], int numberOfArgs)
{
    //initialize ready queue
    initReadyQueue();
    initPID();
    FILE* fileArray[numberOfArgs];
    //check all files can be opened
    for(int i = 1; i < numberOfArgs; i++){
        FILE *fp = fopen(words[i], "rt");
        if(fp == NULL)
        {
            printf("Could not open the selected file: %s\n", words[i]);
            return 0;
        }else{
            fileArray[i] = fp;
        }  
    }

    for(int i = 1; i < numberOfArgs; i++){
        int errorCode = launcher(fileArray[i]);
        if(errorCode == 0){
            printf("Launcher Error occured, BackingStore and RAM cleared\n");
            clearRAM();
            return 0;
        }
    }

    scheduler();
    clearRAM();
    return 0;
}

int interpreter(char *words[], int numberOfArgs)
{
    int errorCode = 0;

    // The user is asking to execute a single command

    if (strcmp(words[0], "help\n") == 0 || strcmp(words[0], "help\r") == 0 || strcmp(words[0], "help") == 0)
    {
        errorCode = help();
    }
    else if (strcmp(words[0], "quit\n") == 0 || strcmp(words[0], "quit\r") == 0 || strcmp(words[0], "quit") == 0)
    {
        errorCode = quit();
    }
    else if (strcmp(words[0], "set") == 0 && numberOfArgs > 2)
    {
        errorCode = set(words);
    }
    else if (strcmp(words[0], "print") == 0 && numberOfArgs == 2)
    {
        errorCode = print(words);
    }
    else if (strcmp(words[0], "run") == 0 && numberOfArgs == 2)
    {
        errorCode = run(words[1]);
    }
    else if (strcmp(words[0], "exec") == 0 && numberOfArgs < 5 && numberOfArgs > 1)
    {
        errorCode = exec(words, numberOfArgs);
    }
    else
    {
        //unknown command must throw an error message
        printf("Unknown command, please type a valid command\n");
        errorCode = 1;
    }
    return errorCode;
}