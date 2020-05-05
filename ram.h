void addToRAM(FILE *p, int *start, int *end);
void removeFromRAM(PCB pcb);
void initRAM();
void clearRAM();
void checkRAM();
char* getInstruction(int i);
void copyToRAM(int i, char *content);
void removeFrameFromRAM(int frameNumber);