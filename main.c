#include <stdio.h>
#include <stdlib.h>
#include "8080_cpu.h"


int main(int argc, char **argv)
{
    Registers *registers = NULL;
    uint8_t done = 0;

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
    if(LoadROM(registers, argv[1], 0) != 0)
    {
        printf("File could no be opened !\n");
        return -1;
    }
    printf("Data loaded successfully!\n");
    printf("Proceeding to dissasembly code...\n\n");

    while (!done)
    {
        done = Emulate8080(registers);
    }
    
}