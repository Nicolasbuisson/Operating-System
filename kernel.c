#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "cpu.h"
#include "pcb.h"
#include "ram.h"
#include "memorymanager.h"

int numOfPCB = 0;

typedef struct ReadyQueue{
    PCB *head;
    PCB *tail; 
}ReadyQueue;

ReadyQueue *ptrReadyQueue = NULL; //Ready Queue pointer

void addToReady(PCB* pcb){
    //adds PCB to the tail of the Ready Queue
    if(ptrReadyQueue->head == NULL){
        //no elements in Queue, so put it to head
        ptrReadyQueue->head = pcb;
        ptrReadyQueue->head->next = NULL;
        ptrReadyQueue->tail = pcb;
    }
    else
    {
        //Queue has a head so it is not empty
        ptrReadyQueue->tail->next = pcb;
        ptrReadyQueue->tail = pcb;
        ptrReadyQueue->tail->next = NULL;
    }
    numOfPCB++;
}

void myinit(FILE *filename){
    
    //add PCB instructions to the RAM
    int start = 0;
    int end = 0;
    addToRAM(filename, &start, &end);
    //makes PCB
    PCB *ptr = makePCB(start, end);
    //adds the PCB to the ReadyQueue
    addToReady(ptr);
}

void removePCBFromQueue(){
    //we only remove a PCB from the Queue once it is done completing
    //so the PCB will always be at the head
    ptrReadyQueue->head = ptrReadyQueue->head->next;
}
void completeExecution(PCB* pcb){
    //remove PCB from queue
    removePCBFromQueue();

    //remove PCB's frames from RAM
    for(int i = 0; i < pcb->pages_max; i++){
        removeFrameFromRAM(pcb->pageTable[i]);
    }

    //delete the file in the backing store
    char filepath[70] = "BackingStore\\";
    char PIDchar[32];
    sprintf(PIDchar, "%d", pcb->PID);
    char *filename = strcat(PIDchar, ".txt");
    strcat(filepath, filename);
    char deleteInstruction[100] = "DEL /F "; 
    strcat(deleteInstruction, filepath);
    system(deleteInstruction);
    //release the pcb
    free(pcb);
    pcb = NULL;
    numOfPCB--;
}

void nextInQueue(){
    //we only dequeue from the head of the queue
    ptrReadyQueue->tail->next = ptrReadyQueue->head;
    ptrReadyQueue->tail = ptrReadyQueue->head;
    ptrReadyQueue->head = ptrReadyQueue->head->next;
}

PCB* getActivePCB(){
    return ptrReadyQueue->head;
}

PCB* findPCBbyFrameNumber(int frameNumber){
    //used when trying to update the victim's pageTable

    //iterates through all the PCB's to find who is
    //the owner of a specific frame
    PCB *next = getActivePCB()->next;
    for(int i = 1; i < numOfPCB; i++){
        if(checkPageTable(next, frameNumber) == 1){
            return next;
        }
        next = next->next;
    }
}

int findPageNumber(PCB *p, int frameNumber){
    //used when trying to update the victim's pageTable

    //need to find which page of the victim was replaced
    for(int i = 0; i < p->pages_max; i++){
        if(p->pageTable[i] == frameNumber){
            return i;
        }
    }
    return -1;
}

void pageFault(){
    PCB* current = getActivePCB();
    //must first check if the next page is loaded into RAM
    //or we must get it from the Backing Store
    current->PC_page++;
    if(current->PC_page > current->pages_max){
        completeExecution(current);
        return;
    }
    if(current->pageTable[current->PC_page] != -1){
        //page is valid and already loaded into RAM
        //current->PC_offset = 0;
        updatePC(current);
        return;
    }else{
        //page is not in RAM, must load it
        int frameLocation = findFrame();
        if(frameLocation == -1){
            //no free frames must select a victim
            frameLocation = findVictim(getActivePCB());
            PCB *victim = findPCBbyFrameNumber(frameLocation);
            int victimPageNumber = findPageNumber(victim, frameLocation);
            // -1 as a frame number indicates the page is no longer in RAM 
            // but must be retrieved from the Backing Store once again
            updatePageTable(victim, victimPageNumber, -1);
            removeFrameFromRAM(frameLocation);
        }
        //load the missing page into RAM and update page table
        FILE *fileptr = fopen(getFileName(current->PID), "r");
        loadPage(current->PC_page, fileptr, frameLocation);
        fclose(fileptr);
        updatePageTable(current, current->PC_page, frameLocation);
    }
    //update PCB values
    updatePC(current);
    //finally, place PCB at back of the Ready Queue, handled by scheduler
}

void scheduler(){
    
    int quitFlag = 0;
    while(numOfPCB > 0){    
        //Take minimum quanta between 2 and 1
        if(ptrReadyQueue->head->max_lines - ptrReadyQueue->head->PC == 1){
            ptrCPU->quanta = 1;
        }else if(ptrReadyQueue->head->max_lines - ptrReadyQueue->head->PC == 0){
            ptrCPU->quanta = 0;
        }else{
            ptrCPU->quanta = 2;
        }
        if(ptrCPU->quanta == 0){
            break;
        }
        //check that page about to be executed is loaded into RAM
        //must do this check since the frame in which it could have
        //previously been loaded could have been stolen by another program 
        if(ptrReadyQueue->head->pageTable[ptrReadyQueue->head->PC_page] == -1){
            //means page is not currently loaded into RAM
            //we must reload it
            ptrReadyQueue->head->PC_page--;
            pageFault();
        }
        //update CPU info with PCB's info before running
        ptrCPU->IP = (ptrReadyQueue->head->pageTable[ptrReadyQueue->head->PC_page])*4;
        ptrCPU->offset = ptrReadyQueue->head->PC_offset;
        quitFlag = run(ptrCPU->quanta);

        //update PCB info
        ptrReadyQueue->head->PC_offset = ptrCPU->offset;
        updatePC(ptrReadyQueue->head);
        
        if(quitFlag == -1){
            //the current program running has executed a quit command
            //it has completed it's execution
            completeExecution(ptrReadyQueue->head);
            //must check if the queue is now empty
            if(numOfPCB == 0){
                //scheduler is done
                break;
            }
        }
        
        else if(ptrReadyQueue->head->PC == ptrReadyQueue->head->max_lines){
            //if program has finished executing
            completeExecution(ptrReadyQueue->head);
            if(numOfPCB == 0){
                //scheduler is done
                break;
            }
        }else{
            //else continue Queue implementation
            ptrReadyQueue->tail->next = ptrReadyQueue->head;
            ptrReadyQueue->tail = ptrReadyQueue->head;
            ptrReadyQueue->head = ptrReadyQueue->head->next;
        }
    }
}

void initReadyQueue(){
    ptrReadyQueue = (ReadyQueue *)malloc(sizeof(ReadyQueue));
    ptrReadyQueue->head = NULL;
    ptrReadyQueue->tail = NULL;
}

int kernel(){
    //start the shell to prompt the user
    shellUI();
}

void boot(){
    //instantiate the kernel data and memory structures
    ptrCPU = initCPU();
    initRAM();
    initFileNameArray();
    //prepare the Backing Store by first deleting the previous one
    system("rmdir /Q /S BackingStore");
    //then create a new one
    system("mkdir BackingStore");
}

int main(){
    int error = 0;
    boot();
    error = kernel();
    return error;
}