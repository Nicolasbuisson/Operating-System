#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h"

PCB* makePCB(int start, int end){
    //creates a PCB instance using malloc
    PCB* PCBptr = (PCB*)malloc(sizeof(PCB)); 
    PCBptr->PC = start;
    PCBptr->start = start;
    PCBptr->end = end;
    PCBptr->next = NULL;
    return PCBptr;
}

PCB* makePCBlauncher(int max_lines, int pages_max, int PID){
    //creates a PCB instance using malloc
    PCB* PCBptr = (PCB*)malloc(sizeof(PCB)); 
    PCBptr->PC = 0;
    PCBptr->PC_page = 0;
    PCBptr->PC_offset = 0;
    PCBptr->next = NULL;
    PCBptr->max_lines = max_lines;
    PCBptr->pages_max = pages_max;
    PCBptr->PID = PID;
    return PCBptr;
}

void updatePC(PCB *p){
    p->PC = 4*(p->PC_page) + p->PC_offset;
}

void pageTableInitializer(PCB *p){
    for(int i = 0; i < 10; i++){
        p->pageTable[i] = -1;
    }
}

void printPageTable(PCB *p){
    for(int i = 0; i < 10; i++){
        printf("pageTable[%d]: %d\n", i, p->pageTable[i]);
    }
}