#ifndef _8080_CPU_H
#define _8080_CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

typedef struct flags_8080
{
    uint8_t	z:1;
	uint8_t	s:1;
	uint8_t	p:1;
	uint8_t	cy:1;
	uint8_t	ac:1;

}Flags;

typedef struct registers_8080
{
    uint8_t a;
    union
    {
        struct
        {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };
    
    union
    {
        struct
        {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };
    union
    {
        struct
        {
            uint8_t l;
            uint8_t h;
        };
        uint16_t hl;
    };

    uint16_t sp;
    uint16_t pc;
    uint8_t *memory;

    Flags flags;

}Registers;


int Disas_8080_opcode(uint8_t*data, int pc);
uint8_t Emulate8080(Registers *registers);
uint8_t LoadROM(Registers *registers, uint8_t *filename, uint16_t offset);
Registers *Init_8080(void); 

#endif
