#include <stdio.h>
#include <stdlib.h>
#include "8080_cpu.h"

int show_debug = 0;

int main(int argc, char **argv)
{
    Registers *registers = NULL;
    uint8_t done = 0;
    long rom_bytes = 0;

    printf("Initialization of 8080 CPU...\n");
    registers = Init_8080();
    if (registers == NULL)
    {
        printf("Initialization of 8080 CPU failed !\n");
        return -1;
    }

    if (argc < 2)
    {
        printf("No input file specified !\n");
        return -1;
    }
    if (argc > 2)
    {
        printf("Ignoring aruments after the first one \n");
    }

    printf("Initialization success !\n");
    printf("Loading data into memory...\n");
    if((rom_bytes = LoadROM(registers, argv[1], 0x100)) == -1)
    {
        printf("File could no be opened !\n");
        return -1;
    }
    else if (rom_bytes == -2)
    {
        printf("ROM Error : ROM too big !\n");
        return -1;
    }
    printf("Data loaded successfully!\n");
    printf("Proceeding to dissasembly code...\n\n");
    registers->memory[0] = 0xc3;
    registers->memory[1] = 0;
    registers->memory[2] = 0x01;
    registers->memory[368] = 0x7;
    registers->memory[0x59c] = 0xc3; //JMP    
    registers->memory[0x59d] = 0xc2;    
    registers->memory[0x59e] = 0x05; 

    while (registers->pc != rom_bytes && !done)
    {
            done = Emulate8080(registers);
            if (show_debug)
                showCPUDebugInfos(registers);
        
    }
    
}