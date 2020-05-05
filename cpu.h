typedef struct CPU{
    int IP;
    int offset;
    char *IR;
    int quanta;
}CPU;
CPU* initCPU();
int run(int quanta);
CPU *ptrCPU;
void clearCPU(CPU* cpu);