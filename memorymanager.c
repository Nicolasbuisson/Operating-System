#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h"
#include "ram.h"
#include "kernel.h"

int PID = 0;
char *fileNameArray[60];

void initPID(){
    PID = 0;
}

int countTotalLines(FILE *f){
    int lineCounter = 0;
    while(!feof(f)){

        char *line = malloc(1000 * (sizeof(char)));
        fgets(line, 1000, f);
        //filter out '\n' characters from the input
        int length = strlen(line);
        if (*(line + length - 1) == '\n' || *(line + length - 1) == '\r')
        {
            *(line + length - 1) = '\0';
        }
        lineCounter++;
        free(line);
        line = NULL;
    }
    fclose(f);
    return lineCounter;
}

int countTotalPages(FILE *f){
    int lineCounter = countTotalLines(f);
    if(lineCounter % 4 == 0){
        return (lineCounter / 4);
    }else{
        return ((lineCounter / 4) + 1);
    }
}

void loadPage(int pageNumber, FILE *f, int frameNumber){
    //f is reinitialized back to it's original value everytime
    int numberOfLines = 0;
    while(!feof(f) && numberOfLines < pageNumber*4){
        //skip lines depending on which page number is needed
        char *line = malloc(1000 * (sizeof(char)));
        fgets(line, 1000, f);
        free(line);
        line = NULL;
        numberOfLines++;
    }
    for(int i = 0; i < 4; i++){
        if(feof(f)){
            return;
        }
        char *line = malloc(1000 * (sizeof(char)));
        fgets(line, 1000, f);
        //filter out '\n' characters
        int length = strlen(line);
        if (*(line + length - 1) == '\n' || *(line + length - 1) == '\r')
        {
            *(line + length - 1) = '\0';
        }
        copyToRAM(frameNumber*4 + i, line);
        free(line);
        line = NULL;
    }
    fclose(f);
}

int findFrame(){
    //finds first available frame whoses contents is NULL
    int i = 0;
    while(i < 10){
        char* contentOfRAM = getInstruction(i*4);
        if(contentOfRAM == NULL){
            return i;
        }
        i++;
    }
    //if there are no free frames
    return -1;
}

int checkPageTable(PCB *p, int frameNumber){
    //checks if the frame is in the PCB's page table
    //returns 1 if it is, 0 if it isn't
    for(int i = 0; i < 10; i++){
        if(frameNumber == p->pageTable[i]){
            return 1;
        }
    }
    return 0;
}

int findVictim(PCB *p){
    //use a random number generator to pick a random frame number
    //generates a random number between 0 and 10
    int random = (rand() % (10 - 0 + 1) + 0);
    if(checkPageTable(p, random) == 1){
        //frame number belongs to the pages of the active PCB
        while(checkPageTable(p, random) == 1){
            random = (random + 1) % 10;
        }
        return random;
    }else{
        //frame number does not belong to the pages of the active PCB
        return random;
    }
}

int updatePageTable(PCB *p, int pageNumber, int frameNumber){
    p->pageTable[pageNumber] = frameNumber;
}

char* getFileName(int i){
    return fileNameArray[i];
}

void initFileNameArray(){
    for(int i = 0; i < 60; i++){
        fileNameArray[i] = NULL;
    }
}

void checkFileNameArray(){
    for(int i = 0; i < 60; i++){
        printf("fileNameArray[%d]: %s\n", i, fileNameArray[i]);
    }
}

void clearBackingStore(){
    for(int i = 0; i <= PID; i++){
            char deleteInstruction[100] = "DEL /F "; 
            strcat(deleteInstruction, getFileName(i));
            system(deleteInstruction);
        }
}

int launcher(FILE *p){
    
    //create and open file in BackingStore
    char filepath[100] = "BackingStore\\";
    char PIDchar[32];
    sprintf(PIDchar, "%d", PID);
    char *filename = strcat(PIDchar, ".txt");
    strcat(filepath, filename);
    FILE *fptr = fopen(filepath, "w+");

    //add newly created file name to fileNameArray of memory manager
    fileNameArray[PID] = strdup(filepath);
    //copy contents into new file in BackingStore
    while(!feof(p)){

        char *lineCopy = malloc(1000 * (sizeof(char)));
        fgets(lineCopy, 1000, p);
        //printf("content of line: %s", line);
        fputs(lineCopy, fptr);
        free(lineCopy);
        lineCopy = NULL;
    }
    //close file pointers
    fclose(p);
    fclose(fptr);
    fptr = fopen(filepath, "r");
    //load 1 or 2 pages of the program into RAM
    int totalPages = countTotalPages(fptr);
    int numberOfPagesToLoad = 0;
    if(totalPages == 1){
        numberOfPagesToLoad = 1;
    }else{
        numberOfPagesToLoad = 2;
    }
    
    //create new PCB
    
    fptr = fopen(filepath, "r");
    int totalLines = countTotalLines(fptr);
    if(totalLines > 40){
        printf("load error: script contains more than 40 lines, not enough RAM\n");
        clearBackingStore();
        return 0;
    }
    PCB *launchedPCB = makePCBlauncher(totalLines, totalPages, PID);
    PID++;
    //initialize its Page Table
    pageTableInitializer(launchedPCB);

    for(int i = 0; i < numberOfPagesToLoad; i++){
        //renew file pointer
        fptr = fopen(filepath, "r");
        //find frame in which to insert
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
        loadPage(i, fptr, frameLocation);
        updatePageTable(launchedPCB, i, frameLocation);
        
    }
    fclose(fptr);
    addToReady(launchedPCB);
    return 1; //if successful
}