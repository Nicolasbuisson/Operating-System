#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "cpu.h"
#include "pcb.h"
#include "kernel.h"

/*
RAM is a global array of size 40 strings
each 4 cells of the array constitute a frame
*/

char *RAM[40];
int numOfInstructions = 0;

void initRAM(){
    for(int i = 0; i < 40; i++){
        RAM[i] = NULL;
    }
}

void checkRAM(){
    for(int i = 0; i < 40; i++){
        printf("RAM[%d] : %s\n", i, RAM[i]);
    }
}

void clearRAM(){
    for(int i = 0; i < 40; i++){
        free(RAM[i]);
        RAM[i] = NULL;
    }
    numOfInstructions = 0;
}

void addToRAM(FILE *p, int *start, int *end){
    
    int i = 0;
    //finds the first empty slot in the array
    while(RAM[i] != NULL && i < 40){
        i++;
    }
    if(i == 40){
        printf("RAM is full, unable to add more instructions\n");
        return;
    }

    *start = numOfInstructions;
    while (!feof(p)) //read from file until empty
    {
        char *line = malloc(1000 * (sizeof(char)));
        fgets(line, 1000, p);
        numOfInstructions++;
        //filter out '\n' characters from the input
        int length = strlen(line);
        if (*(line + length - 1) == '\n' || *(line + length - 1) == '\r')
        {
            *(line + length - 1) = '\0';
        }

        if(numOfInstructions > 40)
        {
            printf("Load error: not enough space to load all instructions, RAM was cleared\n");
            clearRAM();
            return;
        }

        RAM[i] = strdup(line);
        free(line);
        line = NULL;
        i++;
    }
    *end = numOfInstructions-1;
    //checkRAM();   
}

void removeFromRAM(PCB pcb){
    for(int i = pcb.start; i <= pcb.end; i++){
        free(RAM[i]);
        RAM[i] = NULL;
        numOfInstructions--;
    }
    //checkRAM();
}

char* getInstruction(int i){
    return RAM[i];
}

void copyToRAM(int i, char *content){
    RAM[i] = strdup(content);
}

void removeFrameFromRAM(int frameNumber){
    for(int i = frameNumber*4; i < (frameNumber+1)*4; i++){
        free(RAM[i]);
        RAM[i] = NULL;
        numOfInstructions--;
    }
}