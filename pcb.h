typedef struct PCB{
    int PC;
    int start;
    int end;
    int pageTable[10];
    int PC_page;
    int PC_offset;
    int pages_max;
    int max_lines;
    int PID;
    struct PCB *next;
}PCB;
PCB* makePCB(int start, int end);
PCB* makePCBlauncher(int max_lines, int pages_max, int PID);
void updatePC(PCB *p);
void pageTableInitializer(PCB *p);
void printPageTable(PCB *p);