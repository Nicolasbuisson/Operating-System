#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct MEM
{
    /* data */
    char *var;
    char *value;
}MEM;

struct MEM memory[1000]; 
MEM* memorypointer;


void initializeMemory(){

    memorypointer = malloc(sizeof(MEM)*1000);
    MEM* reset = memorypointer;
    for(int i = 0; i<1000; i++){
        memorypointer->var = NULL;
        memorypointer->value = NULL;
        memorypointer++;
    }
    memorypointer = reset;
}


int writingIndex = 0;
int memorySize = 1000;

int write(char *variable, char *value){
    
    //first check if the variable already exists
    //to potentially overwrite its value

    MEM* reset = memorypointer; //saving initial value to later reset pointer before exiting the function
    memorypointer--;
    for(int i = 0; i < writingIndex; i++){
        memorypointer++;
        if(strcmp(memorypointer->var, variable) ==0){
            memorypointer -> value = strdup(value);
            memorypointer = reset; //reinitialize memory pointer back to beginning of array after overwriting new value
            return 0;
        }
    }

    memorypointer = reset;
    
    if(writingIndex == 1000){
        //reached end of memory array
        printf("No more space left in the shell memory, cannot set anymore values\n");
        return 0;
    }
    writingIndex++;

    for(int i = 0; i < memorySize; i++){
        if(memorypointer->var == NULL){
            memorypointer->var = strdup(variable);
            memorypointer -> value = strdup(value);
            memorypointer = reset; //reinitialize memory pointer back to beginning of array after overwriting new value
            return 0;
        }
        memorypointer++;
    }

    memorypointer = reset; //reset pointer before exiting
    return 0;
}

int read(char *variable){

    MEM* reset = memorypointer;

    for(int i = 0; i < writingIndex; i++){
        if(strcmp(memorypointer -> var, variable) == 0){
            printf("%s\n", memorypointer -> value);
            memorypointer = reset;
            return 0;
        }
        memorypointer++;
    }
    //did not find the variable in the memory
    printf("The variable %s is not in the shell memory\n", variable);
    memorypointer = reset;
    return 0;
}