int launcher(FILE* p);
int findVictim(PCB *p);
int checkPageTable(PCB *p, int frameNumber);
int findFrame();
void loadPage(int pageNumber, FILE *f, int frameNumber);
int updatePageTable(PCB *p, int pageNumber, int frameNumber);
void initFileNameArray();
void checkFileNameArray();
char* getFileName(int i);
void initPID();