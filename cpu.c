#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "shell.h"
#include "pcb.h"
#include "ram.h"
#include "kernel.h"
#include "interpreter.h"


CPU* initCPU(){
    ptrCPU = (CPU*)malloc(sizeof(CPU));
    ptrCPU->IP = 0;
    ptrCPU->offset = 0;
    ptrCPU->quanta = 2;
    ptrCPU->IR = NULL;
    return ptrCPU;
}

int run(int quanta){
    int quitDetector = 0;
    for(int i = 0; i < quanta; i++){

       //strdup mallocs automatically
       ptrCPU->IR = strdup(getInstruction(ptrCPU->IP + ptrCPU->offset));
       //printf("address from RAM: %d |||| content of IR: %s\n", ptrCPU->IP + ptrCPU->offset, ptrCPU->IR);
       if(ptrCPU->IR == NULL){
           //means no instruction in the rest of the frame
           //end of program
           quitDetector = -1;
           return quitDetector;
       }
        //Check in case of quit command
        quitDetector = parse(ptrCPU->IR);
        //now iterate to next instruction 
        ptrCPU->offset++;
        free(ptrCPU->IR);
        ptrCPU->IR = NULL;
        if(quitDetector == -1){
            //script has executed a quit command
            break;
        }
        if(ptrCPU->offset == 4){
            //reached end of page
            //page fault sequence and pseudo interrupt
            pageFault();
            ptrCPU->offset = 0;
            break;
        }
    }
    return quitDetector;
}

void clearCPU(CPU* cpu){
    free(cpu);
    cpu = NULL;
}