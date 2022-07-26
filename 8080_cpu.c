#include "8080_cpu.h"

extern int show_debug;

int parity(int x, int size)
{
	int parity = 0;

	int i;
	for (i = 0; i < size; i++)
	{
		parity += x & 1;
		x = x >> 1;
	}
	return (parity % 2 == 0);
}

void ArithupdateFlags(Flags *flags, uint16_t res)
{
    flags->z = (res== 0);
    flags->s = (0x80 == (res & 0x80));
	flags->p = parity(res & 0xff, 8);
	flags->ac = (res > 0x09);
}

void BcdArithFlags(Flags *flags, uint16_t res)
{
	flags->cy = (res > 0xff); //checks if result is greater than 0xff
	flags->z = ((res & 0xff) == 0);
	flags->s = (0x80 == (res & 0x80)); //res & 1000 0000
	flags->p = parity(res & 0xff, 8);
	flags->ac = (res > 0x09);
}

void LDA(Registers *registers, unsigned char *opcode)
{
	uint16_t address = (opcode[2] << 8) | (opcode[1]);
	registers->a = registers->memory[address];
}

void LDAX(Registers *registers, uint16_t *reg)
{
	registers->a = registers->memory[*reg];

}
void DAD(Registers *registers, uint16_t *reg)
{

	registers->flags.cy = ((registers->hl + *reg) >> 16) & 1;
  	registers->hl = registers->hl + *reg;
}

void DAA(Registers *registers)
{
	uint8_t fourLSB = ((registers->a << 4) >> 4);
	if ((fourLSB > 0x09) || (registers->flags.ac == 1))
	{
		uint16_t resultLSB = fourLSB + 0x06;
		registers->a = registers->a + 0x06;
		BcdArithFlags(&registers->flags, resultLSB);
	}

	uint8_t fourMSB = (registers->a >> 4);
	if ((fourMSB > 0x09) || (registers->flags.cy == 1))
	{
		uint16_t resultMSB = fourMSB + 0x06;
		registers->a = registers->a + 0x06;
		BcdArithFlags(&registers->flags, resultMSB);
	}
}

void MOV(uint8_t *dst_reg, uint8_t *src_reg)
{
	*dst_reg = *src_reg;
}
void STAX(Registers *registers, uint16_t *reg)
{
	registers->memory[*reg] = registers->a;
}
void RLC(Registers *registers)
{
    uint8_t tmp = registers->a;
    registers->a = tmp << 1 | tmp >> 7;
    registers->flags.cy = ((tmp >> 7) > 0);
}
void RAL(Registers *registers)
{
	uint8_t temp = registers->a;
	uint8_t msb = (registers->a >> 7);
	registers->a = (temp << 1) | (registers->flags.cy);
	registers->flags.cy = msb;
}
void RAR(Registers *registers)
{
	uint8_t temp = registers->a;
	uint8_t msb = ((registers->a >> 7) << 7);
	registers->a = (temp >> 1) | (msb);
	registers->flags.cy = (temp << 7) >> 7;
}
void RRC(Registers *registers)
{
	uint8_t temp = registers->a;

	registers->a = temp >> 1 | temp << 7;
	registers->flags.cy = ((registers->a >> 7) > 0);
}
void MVI(Registers *registers, uint8_t *reg, uint8_t data)
{
    *reg = data;
    ArithupdateFlags(&registers->flags, (uint16_t)*reg);

}
void INR(Registers *registers, uint8_t *reg)
{
    *reg += 1;
    ArithupdateFlags(&registers->flags, (uint16_t)*reg);
}

void DCR(Registers *registers, uint8_t *reg)
{
    *reg -= 1;
    ArithupdateFlags(&registers->flags, (uint16_t)*reg);
}
void INX(uint16_t *reg)
{
	(*reg)++;
}

void DCX(uint16_t *reg)
{
	(*reg)--;
}
void LXI(unsigned char *opcode, uint16_t *reg)
{
	*reg = opcode[2];
	*reg = (*reg << 8) | opcode[1];
}

void LHLD(Registers *registers, unsigned char *opcode)
{
	uint16_t memLocation = (opcode[2] << 8) | opcode[1];

	registers->hl = *(uint16_t*)(registers->memory + memLocation);
}

void STA(Registers *registers, unsigned char *opcode)
{
	uint16_t adress = (opcode[2] << 8) | opcode[1];
	registers->memory[adress] = registers->a;
}

void SHLD(Registers *registers, unsigned char *opcode)
{
	uint16_t memLocation = (opcode[2] << 8) | opcode[1];

	*(uint16_t*)(registers->memory + memLocation) = registers->hl;
}

void ADD(Registers *registers, uint8_t *reg)
{
	uint16_t res = (uint16_t)registers->a + (uint16_t)*reg;
	BcdArithFlags(&registers->flags, res);
	registers->a = res & 0xff;

}
void ADC(Registers *registers, uint8_t *reg)
{
	uint16_t res = (uint16_t)registers->a + (uint16_t)*reg + (uint16_t)registers->flags.cy;
	BcdArithFlags(&registers->flags, res);
	registers->a = res & 0xff;
}
void SBB(Registers *registers, uint8_t *reg)
{
	uint16_t res = (uint16_t)registers->a - (uint16_t)*reg - (uint16_t)registers->flags.cy;
	BcdArithFlags(&registers->flags, res);
	registers->a = res & 0xff;
}

void SUB(Registers *registers, uint8_t *reg)
{
	uint16_t res = (uint16_t)registers->a - (uint16_t)*reg;
	BcdArithFlags(&registers->flags, res);
	registers->a = res & 0xff;
}

void CALL(Registers *registers, unsigned char *opcode)
{
	uint16_t ret = registers->pc + 2;
	registers->memory[registers->sp - 1] = (ret >> 8) & 0xff;
	registers->memory[registers->sp - 2] = (ret & 0xff);
	registers->sp = registers->sp - 2;
	registers->pc = (opcode[2] << 8) | opcode[1];
}
void JMP(Registers *registers, unsigned char *opcode)
{
	registers->pc = (opcode[2] << 8) | (opcode[1]);
}

void ANA(Registers *registers, uint8_t *reg)
{
	uint16_t res = (uint16_t)registers->a & (uint16_t)*reg;
	BcdArithFlags(&registers->flags, res);
	registers->flags.cy = 0;
	registers->a = res;
}
void XRA(Registers *registers, uint8_t *reg)
{
	uint16_t res = (uint16_t)registers->a ^ (uint16_t)*reg;
	BcdArithFlags(&registers->flags, res);
	registers->flags.cy = 0;
	registers->flags.ac = 0;
	registers->a = res;
}

void ORA(Registers *registers, uint8_t *reg)
{
	uint16_t res = (uint16_t)registers->a | (uint16_t)*reg;
	BcdArithFlags(&registers->flags, res);
	registers->flags.cy = 0;
	registers->flags.ac = 0;
	registers->a = res;
}

void CMP(Registers *registers, uint8_t *reg)
{
	uint16_t res = registers->a - *reg;
	registers->flags.z = (res == 0);
	registers->flags.s = (0x80 == (res & 0x80));
	registers->flags.p = parity(res, 8);
	registers->flags.cy = (registers->a < *reg);
}

void RET(Registers *registers)
{
	registers->pc = (registers->memory[registers->sp + 1]) << 8 | (registers->memory[registers->sp]);
	registers->sp += 2;
}

void POP(Registers *registers, uint16_t *reg)
{

	*reg = registers->memory[registers->sp + 1];
	*reg = (*reg << 8) | registers->memory[registers->sp];

	registers->sp += 2;
}

void PUSH(Registers *registers, uint16_t *reg)
{
	registers->memory[registers->sp - 1] = (uint8_t)(*reg >> 8);
	registers->memory[registers->sp - 2] = (uint8_t)(*reg);

	registers->sp -= 2;
}

void PUSH_PSW(Registers *registers)
{

	uint16_t psw = 0;
	psw |= registers->flags.s << 7;
	psw |= registers->flags.z << 6;
	psw |= registers->flags.ac << 4;
	psw |= registers->flags.p << 2;
	psw |= 1 << 1; // bit 1 is always 1
	psw |= registers->flags.cy << 0;

	psw = registers->a << 8 | psw;
	PUSH(registers, (uint16_t*)&psw);
	
}

void POP_PSW(Registers *registers)
{
	uint16_t af;
	uint8_t psw;

	POP(registers, &af);
	registers->a = af >> 8;
	psw = af & 0xFF;

	registers->flags.s = (psw >> 7) & 1;
	registers->flags.z = (psw >> 6) & 1;
	registers->flags.ac = (psw >> 4) & 1;
	registers->flags.p = (psw >> 2) & 1;
	registers->flags.cy= (psw >> 0) & 1;
	
}
void XCHG(Registers *registers)
{
	uint16_t tmp = registers->de;

	registers->de = registers->hl;
	registers->hl = tmp;
}

void XTHL(Registers *registers)
{
	uint16_t tmp = registers->memory[registers->sp + 1] << 8;
	tmp |= registers->memory[registers->sp];

	registers->memory[registers->sp] = registers->l;
	registers->memory[registers->sp + 1] = registers->h;

	registers->hl = tmp;


}

void UnimplementedInstruction(Registers* registers)
{
	//pc will have advanced one, so undo that
	printf ("Error: Unimplemented instruction\n");
	//registers->pc--;
	printf("\n");
}

int Disas_8080_opcode(unsigned char *buffer, int pc)
{
    int opbytes = 1;

    unsigned char *opcode = &buffer[pc];

    switch (*opcode)
    {
        case 0x00:
            printf("NOP");
            break;
        case 0x01:
            printf("LXI\tB, $0x%02x%02x", opcode[2], opcode[1]);
            opbytes=3;
            break;
        
        case 0x02:
            printf("STAX\tB");
            break;

        case 0x03:
            printf("INX\tB");
            break;
        
        case 0x04:
            printf("INR\tB");
            break;

        case 0x05:
            printf("DCR\tB");
            break;
        
        case 0x06:
			printf("MVI\tB, $0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0x07:
			printf("RLC");
			break;
		case 0x09:
			printf("DAD\tB");
			break;
		case 0x0a:
			printf("LDAX\tB");
			break;
		case 0x0b:
			printf("DCX\tB");
			break;
		case 0x0c:
			printf("INR\tC");
			break;
		case 0x0d:
			printf("DCR\tC");
			break;
		case 0x0e:
			printf("MVI\tC, $0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0x0f:
			printf("RRC");
			break;
		case 0x11:
			printf("LXI\tD, $0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0x12:
			printf("STAX\tD");
			break;
		case 0x13:
			printf("INX\tD");
			break;
		case 0x14:
			printf("INR\tD");
			break;
		case 0x15:
			printf("DCR\tD");
			break;
		case 0x16:
			printf("MVI\tD, $0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0x17:
			printf("RAL");
			break;
		case 0x19:
			printf("DAD\tD");
			break;
		case 0x1a:
			printf("LDAX\tD");
			break;
		case 0x1b:
			printf("DCX\tD");
			break;
		case 0x1c:
			printf("INR\tE");
			break;
		case 0x1d:
			printf("DCR\tE");
			break;
		case 0x1e:
			printf("MVI\tE, $0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0x1f:
			printf("RAR");
			break;
		case 0x20:
			printf("RIM");
			break;
		case 0x21:
			printf("LXI\tH, $0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0x22:
			printf("SHLD\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0x23:
			printf("INX\tH");
			break;
		case 0x24:
			printf("INR\tH");
			break;
		case 0x25:
			printf("DCR\tH");
			break;
		case 0x26:
			printf("MVI\tH, $0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0x27:
			printf("DAA");
			break;
		case 0x29:
			printf("DAD\tH");
			break;
		case 0x2a:
			printf("LHLD\t 0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0x2b:
			printf("DCX\tH");
			break;
		case 0x2c:
			printf("INR\tL");
			break;
		case 0x2d:
			printf("DCR\tL");
			break;
		case 0x2e:
			printf("MVI\tL, $0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0x2f:
			printf("CMA");
			break;
		case 0x30:
			printf("SIM");
			break;
		case 0x31:
			printf("LXI\tSP, $0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0x32:
			printf("STA\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0x33:
			printf("INX\tSP");
			break;
		case 0x34:
			printf("INR\tM");
			break;
		case 0x35:
			printf("DCR\tM");
			break;
		case 0x36:
			printf("MVI\tM, $0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0x37:
			printf("STC");
			break;
		case 0x39:
			printf("DAD\tSP");
			break;
		case 0x3a:
			printf("LDA\t0x%02x%02X", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0x3b:
			printf("DCX\tSP");
			break;
		case 0x3c:
			printf("INR\tA");
			break;
        case 0x3d:
			printf("DCR\tA");
			break;
		case 0x3e:
			printf("MVI\tA, $0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0x3f:
			printf("CMC");
			break;
		case 0x40:
			printf("MOV\tB, B");
			break;
		case 0x41:
			printf("MOV\tB, C");
			break;
		case 0x42:
			printf("MOV\tB, D");
			break;
		case 0x43:
			printf("MOV\tB, E");
			break;
		case 0x44:
			printf("MOV\tB, H");
			break;
		case 0x45:
			printf("MOV\tB, L");
			break;
		case 0x46:
			printf("MOV\tB, M");
			break;
		case 0x47:
			printf("MOV\tB, A");
			break;
		case 0x48:
			printf("MOV\tC, B");
			break;
		case 0x49:
			printf("MOV\tC, C");
			break;
		case 0x4a:
			printf("MOV\tC, D");
			break;
		case 0x4b:
			printf("MOV\tC, E");
			break;
		case 0x4c:
			printf("MOV\tC, H");
			break;
		case 0x4d:
			printf("MOV\tC, L");
			break;
		case 0x4e:
			printf("MOV\tC, M");
			break;
		case 0x4f:
			printf("MOV\tC, A");
			break;
		case 0x50:
			printf("MOV\tD, B");
			break;
		case 0x51:
			printf("MOV\tD, C");
			break;
		case 0x52:
			printf("MOV\tD, D");
			break;
		case 0x53:
			printf("MOV\tD, E");
			break;
		case 0x54:
			printf("MOV\tD, H");
			break;
		case 0x55:
			printf("MOV\tD, L");
			break;
		case 0x56:
			printf("MOV\tD, M");
			break;
		case 0x57:
			printf("MOV\tD, A");
			break;
		case 0x58:
			printf("MOV\tE, B");
			break;
		case 0x59:
			printf("MOV\tE, C");
			break;
		case 0x5a:
			printf("MOV\tE, D");
			break;
		case 0x5b:
			printf("MOV\tE, E");
			break;
		case 0x5c:
			printf("MOV\tE, H");
			break;
		case 0x5d:
			printf("MOV\tE, L");
			break;
		case 0x5e:
			printf("MOV\tE, M");
			break;
		case 0x5f:
			printf("MOV\tE, A");
			break;
		case 0x60:
			printf("MOV\tH, B");
			break;
		case 0x61:
			printf("MOV\tH, C");
			break;
		case 0x62:
			printf("MOV\tH, D");
			break;
		case 0x63:
			printf("MOV\tH, E");
			break;
		case 0x64:
			printf("MOV\tH, H");
			break;
		case 0x65:
			printf("MOV\tH, L");
			break;
		case 0x66:
			printf("MOV\tH, M");
			break;
		case 0x67:
			printf("MOV\tH, A");
			break;
		case 0x68:
			printf("MOV\tL, B");
			break;
		case 0x69:
			printf("MOV\tL, C");
			break;
		case 0x6a:
			printf("MOV\tL, D");
			break;
		case 0x6b:
			printf("MOV\tL, E");
			break;
		case 0x6c:
			printf("MOV\tL, H");
			break;
		case 0x6d:
			printf("MOV\tL, L");
			break;
		case 0x6e:
			printf("MOV\tL, M");
			break;
		case 0x6f:
			printf("MOV\tL, A");
			break;
		case 0x70:
			printf("MOV\tM, B");
			break;
		case 0x71:
			printf("MOV\tM, C");
			break;
		case 0x72:
			printf("MOV\tM, D");
			break;
		case 0x73:
			printf("MOV\tM, E");
			break;
		case 0x74:
			printf("MOV\tM, H");
			break;
		case 0x75:
			printf("MOV\tM, L");
			break;
		case 0x76:
			printf("HLT");
			break;
		case 0x77:
			printf("MOV\tM, A");
			break;
		case 0x78:
			printf("MOV\tA, B");
			break;
		case 0x79:
			printf("MOV\tA, C");
			break;
		case 0x7a:
			printf("MOV\tA, D");
			break;
		case 0x7b:
			printf("MOV\tA, E");
			break;
		case 0x7c:
			printf("MOV\tA, H");
			break;
		case 0x7d:
			printf("MOV\tA, L");
			break;
		case 0x7e:
			printf("MOV\tA, M");
			break;
		case 0x7f:
			printf("MOV\tA, A");
			break;
		case 0x80:
			printf("ADD\tB");
			break;
		case 0x81:
			printf("ADD\tC");
			break;
		case 0x82:
			printf("ADD\tD");
			break;
		case 0x83:
			printf("ADD\tE");
			break;
		case 0x84:
			printf("ADD\tH");
			break;
		case 0x85:
			printf("ADD\tL");
			break;
		case 0x86:
			printf("ADD\tM");
			break;
		case 0x87:
			printf("ADD\tA");
			break;
		case 0x88:
			printf("ADC\tB");
			break;
		case 0x89:
			printf("ADC\tC");
			break;
		case 0x8a:
			printf("ADC\tE");
			break;
		case 0x8b:
			printf("ADC\tE");
			break;
		case 0x8c:
			printf("ADC\tH");
			break;
		case 0x8d:
			printf("ADC\tL");
			break;
		case 0x8e:
			printf("ADC\tM");
			break;
		case 0x8f:
			printf("ADC\tA");
			break;
		case 0x90:
			printf("SUB\tB");
			break;
		case 0x91:
			printf("SUB\tC");
			break;
		case 0x92:
			printf("SUB\tD");
			break;
		case 0x93:
			printf("SUB\tE");
			break;
		case 0x94:
			printf("SUB\tH");
			break;
		case 0x95:
			printf("SUB\tL");
			break;
		case 0x96:
			printf("SUB\tM");
			break;
		case 0x97:
			printf("SUB\tA");
			break;
		case 0x98:
			printf("SBB\tB");
			break;
		case 0x99:
			printf("SBB\tC");
			break;
		case 0x9a:
			printf("SBB\tD");
			break;
		case 0x9b:
			printf("SBB\tE");
			break;
		case 0x9c:
			printf("SBB\tH");
			break;
		case 0x9d:
			printf("SBB\tL");
			break;
		case 0x9e:
			printf("SBB\tM");
			break;
		case 0x9f:
			printf("SBB\tA");
			break;
		case 0xa0:
			printf("ANA\tB");
			break;
		case 0xa1:
			printf("ANA\tC");
			break;
		case 0xa2:
			printf("ANA\tD");
			break;
		case 0xa3:
			printf("ANA\tE");
			break;
		case 0xa4:
			printf("ANA\tH");
			break;
		case 0xa5:
			printf("ANA\tL");
			break;
		case 0xa6:
			printf("ANA\tM");
			break;
		case 0xa7:
			printf("ANA\tA");
			break;
		case 0xa8:
			printf("XRA\tB");
			break;
		case 0xa9:
			printf("XRA\tC");
			break;
		case 0xaa:
			printf("XRA\tD");
			break;
		case 0xab:
			printf("XRA\tE");
			break;
		case 0xac:
			printf("XRA\tH");
			break;
		case 0xad:
			printf("XRA\tL");
			break;
		case 0xae:
			printf("XRA\tM");
			break;
		case 0xaf:
			printf("XRA\tA");
			break;
		case 0xb0:
			printf("ORA\tB");
			break;
		case 0xb1:
			printf("ORA\tC");
			break;
		case 0xb2:
			printf("ORA\tD");
			break;
		case 0xb3:
			printf("ORA\tE");
			break;
		case 0xb4:
			printf("ORA\tH");
			break;
		case 0xb5:
			printf("ORA\tL");
			break;
		case 0xb6:
			printf("ORA\tM");
			break;
		case 0xb7:
			printf("ORA\tA");
			break;
		case 0xb8:
			printf("CMP\tB");
			break;
		case 0xb9:
			printf("CMP\tC");
			break;
		case 0xba:
			printf("CMP\tD");
			break;
		case 0xbb:
			printf("CMP\tE");
			break;
		case 0xbc:
			printf("CMP\tH");
			break;
		case 0xbd:
			printf("CMP\tL");
			break;
		case 0xbe:
			printf("CMP\tM");
			break;
		case 0xbf:
			printf("CMP\tA");
			break;
		case 0xc0:
			printf("RNZ");
			break;
		case 0xc1:
			printf("POP\tB");
			break;
		case 0xc2:
			printf("JNZ\t0x%02x%02X", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xc3:
			printf("JMP\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xc4:
			printf("CNZ\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xc5:
			printf("PUSH\tB");
			break;
		case 0xc6:
			printf("ADI\t$0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0xc7:
			printf("RST\t0");
			break;
		case 0xc8:
			printf("RZ");
			break;
		case 0xc9:
			printf("RET");
			break;
		case 0xca:
			printf("JZ\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xcc:
			printf("CZ\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xcd:
			printf("CALL\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xce:
			printf("ACI\t$0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0xcf:
			printf("RST\t1");
			break;
		case 0xd0:
			printf("RNC");
			break;
		case 0xd1:
			printf("POP\tD");
			break;
		case 0xd2:
			printf("JNC\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xd3:
			printf("OUT\t$0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0xd4:
			printf("CNC\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xd5:
			printf("PUSH\tD");
			break;
		case 0xd6:
			printf("SUI\t$0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0xd7:
			printf("RST\t2");
			break;
		case 0xd8:
			printf("RC");
			break;
		case 0xda: 
			printf("JC\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xdb:
			printf("IN\t$0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0xdc:
			printf("CC\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xde:
			printf("SBI\t$0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0xdf:
			printf("RST\t3");
			break;
		case 0xe0:
			printf("RPO");
			break;
		case 0xe1:
			printf("POP\tH");
			break;
		case 0xe2:
			printf("JPO\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xe3:
			printf("XTHL");
			break;
		case 0xe4:
			printf("CPO\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xe5:
			printf("PUSH\tH");
			break;
		case 0xe6:
			printf("ANI\t$0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0xe7:
			printf("RST\t4");
			break;
		case 0xe8:
			printf("RPE");
			break;
		case 0xe9:
			printf("PCHL");
			break;
		case 0xea:
			printf("JPE\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xeb:
			printf("XCHG");
			break;
		case 0xec:
			printf("CPE\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xee:
			printf("XRI\t$0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0xef:
			printf("RST\t5");
			break;
		case 0xf0:
			printf("RP");
			break;
		case 0xf1:
			printf("POP\tPSW");
			break;
		case 0xf2:
			printf("JP\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xf3:
			printf("DI");
			break;
		case 0xf4: 
			printf("CP\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xf5:
			printf("PUSH\tPSW");
			break;
		case 0xf6:
			printf("ORD\t$0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0xf7:
			printf("RST\t6");
			break;
		case 0xf8:
			printf("RM");
			break;
		case 0xf9:
			printf("SPHL");
			break;
		case 0xfa:
			printf("JM\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xfb:
			printf("EI");
			break;
		case 0xfc:
			printf("CM\t0x%02x%02x", opcode[2], opcode[1]);
			opbytes = 3;
			break;
		case 0xfe:
			printf("CPI\t$0x%02x", opcode[1]);
			opbytes = 2;
			break;
		case 0xff:
			printf("RST\t7");
			break;

		default:
			printf("Unknown Instruction: 0x%02x", *opcode);
			break;
    }

    printf("\n");
    return opbytes;
}

Registers *Init_8080(void)
{
    Registers *registers = (Registers*)calloc(1,sizeof(Registers));
    if (registers != NULL)
    {
        registers->memory = (uint8_t*)malloc(sizeof(uint8_t)*0x10000); // 16K memory
        if (registers->memory == NULL)
            return NULL;

		registers->interrupts = 1;
		registers->pc = 0;
		registers->sp = 0xf000;
        return registers;
    }
    return NULL;

}

uint8_t Emulate8080(Registers *registers)
{
	unsigned char tmp[3] = {0}; 
    uint8_t *opcode = &registers->memory[registers->pc];
	if (show_debug)
	{
		printf("%04x\t", registers->pc);
    	Disas_8080_opcode(registers->memory, registers->pc);
	}

    registers->pc+=1;
    switch (*opcode)
    {
        case 0x00:
            break;
        case 0x01:
            LXI(opcode, &registers->bc);
			registers->pc += 2;
            break;
        case 0x02:
            STAX(registers, &registers->bc);
            break;
        case 0x03:
            INX(&registers->bc);
            break;
        case 0x04:
            INR(registers, &registers->b);
            break;
        case 0x05:
            DCR(registers, &registers->b);
            break;
        case 0x06:
            MVI(registers, &registers->b, opcode[1]);
			registers->pc++;
			break;
		case 0x07:
			RLC(registers);
			break;
		case 0x09:
			DAD(registers, &registers->bc);
			break;
		case 0x0a:
			LDAX(registers, &registers->bc);
			break;
		case 0x0b:
			DCX(&registers->bc);
			break;
		case 0x0c:
			INR(registers, &registers->c);
			break;
		case 0x0d:
			DCR(registers, &registers->c);
			break;
		case 0x0e:
			MVI(registers, &registers->c, opcode[1]);
			registers->pc++;
			break;
		case 0x0f:
			RRC(registers);
			break;
		case 0x11:
			LXI(opcode, &registers->de);
			registers->pc += 2;
			break;
		case 0x12:
			STAX(registers, &registers->de);
			break;
		case 0x13:
			INX(&registers->de);
			break;
		case 0x14:
			INR(registers, &registers->d);
			break;
		case 0x15:
			DCR(registers, &registers->d);
			break;
		case 0x16:
			MVI(registers, &registers->d, opcode[1]);
			registers->pc++;
			break;
		case 0x17:
			RAL(registers);
			break;
		case 0x19:
			DAD(registers, &registers->de);
			break;
		case 0x1a:
			LDAX(registers, &registers->de);
			break;
		case 0x1b:
			DCX(&registers->de);
			break;
		case 0x1c:
			INR(registers, &registers->e);
			break;
		case 0x1d:
			DCR(registers, &registers->e);
			break;
		case 0x1e:
			MVI(registers, &registers->e, opcode[1]);
			registers->pc++;
			break;
		case 0x1f:
			RAR(registers);
			break;
		case 0x20:
			/* No instruction */
			break;
		case 0x21:
			LXI(opcode, &registers->hl);
			registers->pc += 2;
			break;
		case 0x22:
			SHLD(registers, opcode);
			registers->pc += 2;
			break;
		case 0x23:
			INX(&registers->hl);
			break;
		case 0x24:
			INR(registers, &registers->h);
			break;
		case 0x25:
			DCR(registers, &registers->h);
			break;
		case 0x26:
			MVI(registers, &registers->h, opcode[1]);
			registers->pc++;
			break;
		case 0x27:
			DAA(registers);
			break;
		case 0x29:
			DAD(registers, &registers->hl);
			break;
		case 0x2a:
			LHLD(registers, opcode);
			registers->pc += 2;
			break;
		case 0x2b:
			DCX(&registers->hl);
			break;
		case 0x2c:
			INR(registers, &registers->l);
			break;
		case 0x2d:
			DCR(registers, &registers->l);
			break;
		case 0x2e:
			MVI(registers, &registers->l, opcode[1]);
			registers->pc++;
			break;
		case 0x2f:
			registers->a = ~(registers->a);
			break;
		case 0x30:
			/* No instruction */
			//UnimplementedInstruction(registers);
			break;
		case 0x31:
			LXI(opcode, &registers->sp);
			registers->pc += 2;
			break;
		case 0x32:
			STA(registers, opcode);
			registers->pc += 2;
			break;
		case 0x33:
			INX(&registers->sp);
			break;
		case 0x34:
			INR(registers, &registers->memory[registers->hl]);
			break;
		case 0x35:
			DCR(registers, &registers->memory[registers->hl]);
			break;
		case 0x36:
			MVI(registers, &registers->memory[registers->hl], opcode[1]);
			break;
		case 0x37:
			registers->flags.cy = 1;
			break;
		case 0x39:
			DAD(registers, &registers->sp);
			break;
		case 0x3a:
			LDA(registers, opcode);
			registers->pc += 2;
			break;
		case 0x3b:
			DCX(&registers->sp);
			break;
		case 0x3c:
			INR(registers, &registers->a);
			break;
        case 0x3d:
			DCR(registers, &registers->a);
			break;
		case 0x3e:
			MVI(registers, &registers->a, opcode[1]);
			registers->pc += 1;
			break;
		case 0x3f:
			registers->flags.cy = ~registers->flags.cy;
			break;
		case 0x40:
			MOV(&registers->b, &registers->b);
			break;
		case 0x41:
			MOV(&registers->b, &registers->c);
			break;
		case 0x42:
			MOV(&registers->b, &registers->d);
			break;
		case 0x43:
			MOV(&registers->b, &registers->e);
			break;
		case 0x44:
			MOV(&registers->b, &registers->h);
			break;
		case 0x45:
			MOV(&registers->b, &registers->l);
			break;
		case 0x46:
			MOV(&registers->b, &registers->memory[registers->hl]);
			break;
		case 0x47:
			MOV(&registers->b, &registers->a);
			break;
		case 0x48:
			MOV(&registers->c, &registers->b);
			break;
		case 0x49:
			MOV(&registers->c, &registers->c);
			break;
		case 0x4a:
			MOV(&registers->c, &registers->d);
			break;
		case 0x4b:
			MOV(&registers->c, &registers->e);
			break;
		case 0x4c:
			MOV(&registers->c, &registers->h);
			break;
		case 0x4d:
			MOV(&registers->c, &registers->l);
			break;
		case 0x4e:
			MOV(&registers->c, &registers->memory[registers->hl]);
			break;
		case 0x4f:
			MOV(&registers->c, &registers->a);
			break;
		case 0x50:
			MOV(&registers->d, &registers->b);
			break;
		case 0x51:
			MOV(&registers->d, &registers->c);
			break;
		case 0x52:
			MOV(&registers->d, &registers->d);
			break;
		case 0x53:
			MOV(&registers->d, &registers->e);
			break;
		case 0x54:
			MOV(&registers->d, &registers->h);
			break;
		case 0x55:
			MOV(&registers->d, &registers->l);
			break;
		case 0x56:
			MOV(&registers->d, &registers->memory[registers->hl]);
			break;
		case 0x57:
			MOV(&registers->d, &registers->a);
			break;
		case 0x58:
			MOV(&registers->e, &registers->b);
			break;
		case 0x59:
			MOV(&registers->e, &registers->c);
			break;
		case 0x5a:
			MOV(&registers->e, &registers->d);
			break;
		case 0x5b:
			MOV(&registers->e, &registers->e);
			break;
		case 0x5c:
			MOV(&registers->e, &registers->h);
			break;
		case 0x5d:
			MOV(&registers->e, &registers->l);
			break;
		case 0x5e:
			MOV(&registers->e, &registers->memory[registers->hl]);
			break;
		case 0x5f:
			MOV(&registers->e, &registers->a);
			break;
		case 0x60:
			MOV(&registers->e, &registers->b);
			break;
		case 0x61:
			MOV(&registers->h, &registers->b);
			break;
		case 0x62:
			MOV(&registers->h, &registers->d);
			break;
		case 0x63:
			MOV(&registers->h, &registers->e);
			break;
		case 0x64:
			MOV(&registers->h, &registers->h);
			break;
		case 0x65:
			MOV(&registers->e, &registers->l);
			break;
		case 0x66:
			MOV(&registers->e, &registers->memory[registers->hl]);
			break;
		case 0x67:
			MOV(&registers->e, &registers->a);
			break;
		case 0x68:
			MOV(&registers->l, &registers->b);
			break;
		case 0x69:
			MOV(&registers->l, &registers->c);
			break;
		case 0x6a:
			MOV(&registers->l, &registers->d);
			break;
		case 0x6b:
			MOV(&registers->l, &registers->e);
			break;
		case 0x6c:
			MOV(&registers->l, &registers->h);
			break;
		case 0x6d:
			MOV(&registers->l, &registers->l);
			break;
		case 0x6e:
			MOV(&registers->l, &registers->memory[registers->hl]);
			break;
		case 0x6f:
			MOV(&registers->l, &registers->a);
			break;
		case 0x70:
			MOV(&registers->memory[registers->hl], &registers->b);
			break;
		case 0x71:
			MOV(&registers->memory[registers->hl], &registers->c);
			break;
		case 0x72:
			MOV(&registers->memory[registers->hl], &registers->d);
			break;
		case 0x73:
			MOV(&registers->memory[registers->hl], &registers->e);
			break;
		case 0x74:
			MOV(&registers->memory[registers->hl], &registers->h);
			break;
		case 0x75:
			MOV(&registers->memory[registers->hl], &registers->l);
			break;
		case 0x76:
			exit(0);
			break;
		case 0x77:
			MOV(&registers->memory[registers->hl], &registers->a);
			break;
		case 0x78:
			MOV(&registers->a, &registers->b);
			break;
		case 0x79:
			MOV(&registers->a, &registers->c);
			break;
		case 0x7a:
			MOV(&registers->a, &registers->d);
			break;
		case 0x7b:
			MOV(&registers->a, &registers->e);
			break;
		case 0x7c:
			MOV(&registers->a, &registers->h);
			break;
		case 0x7d:
			MOV(&registers->a, &registers->l);
			break;
		case 0x7e:
			MOV(&registers->a, &registers->memory[registers->hl]);
			break;
		case 0x7f:
			MOV(&registers->a, &registers->a);
			break;
		case 0x80:
			ADD(registers, &registers->b);
			break;
		case 0x81:
			ADD(registers, &registers->c);
			break;
		case 0x82:
			ADD(registers, &registers->d);
			break;
		case 0x83:
			ADD(registers, &registers->e);
			break;
		case 0x84:
			ADD(registers, &registers->h);
			break;
		case 0x85:
			ADD(registers, &registers->l);
			break;
		case 0x86:
			ADD(registers, &registers->memory[registers->hl]);
			break;
		case 0x87:
			ADD(registers, &registers->a);
			break;
		case 0x88:
			ADC(registers, &registers->b);
			break;
		case 0x89:
			ADC(registers, &registers->c);
			break;
		case 0x8a:
			ADC(registers, &registers->d);
			break;
		case 0x8b:
			ADC(registers, &registers->e);
			break;
		case 0x8c:
			ADC(registers, &registers->h);
			break;
		case 0x8d:
			ADC(registers, &registers->l);
			break;
		case 0x8e:
			ADC(registers, &registers->memory[registers->hl]);
			break;
		case 0x8f:
			ADC(registers, &registers->a);
			break;
		case 0x90:
			SUB(registers, &registers->b);
			break;
		case 0x91:
			SUB(registers, &registers->c);
			break;
		case 0x92:
			SUB(registers, &registers->d);
			break;
		case 0x93:
			SUB(registers, &registers->e);
			break;
		case 0x94:
			SUB(registers, &registers->h);
			break;
		case 0x95:
			SUB(registers, &registers->l);
			break;
		case 0x96:
			SUB(registers, &registers->memory[registers->hl]);
			break;
		case 0x97:
			SUB(registers, &registers->a);
			break;
		case 0x98:
			SBB(registers, &registers->b);
			break;
		case 0x99:
			SBB(registers, &registers->c);
			break;
		case 0x9a:
			SBB(registers, &registers->d);
			break;
		case 0x9b:
			SBB(registers, &registers->e);
			break;
		case 0x9c:
			SBB(registers, &registers->h);
			break;
		case 0x9d:
			SBB(registers, &registers->l);
			break;
		case 0x9e:
			SBB(registers, &registers->memory[registers->hl]);
			break;
		case 0x9f:
			SBB(registers, &registers->a);
			break;
		case 0xa0:
			ANA(registers, &registers->b);
			break;
		case 0xa1:
			ANA(registers, &registers->c);
			break;
		case 0xa2:
			ANA(registers, &registers->d);
			break;
		case 0xa3:
			ANA(registers, &registers->e);
			break;
		case 0xa4:
			ANA(registers, &registers->h);
			break;
		case 0xa5:
			ANA(registers, &registers->l);
			break;
		case 0xa6:
			ANA(registers, &registers->memory[registers->hl]);
			break;
		case 0xa7:
			ANA(registers, &registers->a);
			break;
		case 0xa8:
			XRA(registers, &registers->b);
			break;
		case 0xa9:
			XRA(registers, &registers->c);
			break;
		case 0xaa:
			XRA(registers, &registers->d);
			break;
		case 0xab:
			XRA(registers, &registers->e);
			break;
		case 0xac:
			XRA(registers, &registers->h);
			break;
		case 0xad:
			XRA(registers, &registers->l);
			break;
		case 0xae:
			XRA(registers, &registers->memory[registers->hl]);
			break;
		case 0xaf:
			XRA(registers, &registers->a);
			break;
		case 0xb0:
			ORA(registers, &registers->b);
			break;
		case 0xb1:
			ORA(registers, &registers->c);
			break;
		case 0xb2:
			ORA(registers, &registers->d);
			break;
		case 0xb3:
			ORA(registers, &registers->e);
			break;
		case 0xb4:
			ORA(registers, &registers->h);
			break;
		case 0xb5:
			ORA(registers, &registers->l);
			break;
		case 0xb6:
			ORA(registers, &registers->memory[registers->hl]);
			break;
		case 0xb7:
			ORA(registers, &registers->a);
			break;
		case 0xb8:
			CMP(registers, &registers->b);
			break;
		case 0xb9:
			CMP(registers, &registers->c);
			break;
		case 0xba:
			CMP(registers, &registers->d);
			break;
		case 0xbb:
			CMP(registers, &registers->e);
			break;
		case 0xbc:
			CMP(registers, &registers->h);
			break;
		case 0xbd:
			CMP(registers, &registers->l);
			break;
		case 0xbe:
			CMP(registers, &registers->memory[registers->hl]);
			break;
		case 0xbf:
			CMP(registers, &registers->a);
			break;
		case 0xc0:
			if (registers->flags.z == 0)
				RET(registers);

			break;
		case 0xc1:
			POP(registers, &registers->bc);
			break;
		case 0xc2:
			if (registers->flags.z == 0)
				JMP(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xc3:
			JMP(registers, opcode);
			break;
		case 0xc4:
			if (registers->flags.z == 0)
				CALL(registers, opcode);
			else
				registers->pc += 2;

			break;
		case 0xc5:
			PUSH(registers, &registers->bc);
			break;
		case 0xc6:
			ADD(registers, &opcode[1]);
			registers->pc++;
			break;
		case 0xc7:
			tmp[1] = 0;
			tmp[2] = 0;
			CALL(registers, 0x0);
			break;
		case 0xc8:
			if (registers->flags.z == 1)
				RET(registers);
			break;
		case 0xc9:
			RET(registers);
			break;
		case 0xca:
			if (registers->flags.z == 1)
				JMP(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xcc:
			if (registers->flags.z == 1)
				CALL(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xcd:
			if (5 ==  ((opcode[2] << 8) | opcode[1]))    
            {    
                if (registers->c == 9)    
                {    
                    uint16_t offset = (registers->d<<8) | (registers->e);    
                    unsigned char *str = &registers->memory[offset+3];  //skip the prefix bytes    
                    while (*str != '$')    
                        printf("%c", *str++);    
                    printf("\n");    
                }    
                else if (registers->c == 2)    
                {    
                    //saw this in the inspected code, never saw it called    
                    printf ("print char routine called\n");    
                }    
            }    
            else if (0 ==  ((opcode[2] << 8) | opcode[1]))    
            {    
                exit(0);    
            }
			else
				CALL(registers, opcode);
			break;
		case 0xce:
			ADC(registers, &opcode[1]);
			registers->pc++;
			break;
		case 0xcf:
			tmp[1] = 8;
			tmp[2] = 0;
			CALL(registers, tmp);
			break;
		case 0xd0:
			if (registers->flags.cy == 0)
				RET(registers);
			break;
		case 0xd1:
			POP(registers, &registers->de);
			break;
		case 0xd2:
			if (registers->flags.cy == 0)
				JMP(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xd3:
			break;
		case 0xd4:
			if (registers->flags.cy == 0)
				CALL(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xd5:
			PUSH(registers, &registers->de);
			break;
		case 0xd6:
			SUB(registers, &opcode[1]);
			registers->pc++;
			break;
		case 0xd7:
			tmp[1] = 16;
			tmp[2] = 0;
			CALL(registers, tmp);
			break;
		case 0xd8:
			if (registers->flags.cy == 1)
				RET(registers);
			break;
		case 0xda: 
			if (registers->flags.cy == 1)
				JMP(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xdb:
			break;
		case 0xdc:
			if (registers->flags.cy == 1)
				CALL(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xde:
			SBB(registers, &opcode[1]);
			registers->pc++;
			break;
		case 0xdf:
			tmp[1] = 24;
			tmp[2] = 0;
			CALL(registers, tmp);
			break;
		case 0xe0:
			if (registers->flags.p == 0)
				RET(registers);
			break;
		case 0xe1:
			POP(registers, &registers->hl);
			break;
		case 0xe2:
			if (registers->flags.p == 0)
				JMP(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xe3:
			XTHL(registers);
			break;
		case 0xe4:
			if (registers->flags.p == 0)
				CALL(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xe5:
			PUSH(registers, &registers->hl);
			break;
		case 0xe6:
			ANA(registers, &opcode[1]);
			registers->pc++;
			break;
		case 0xe7:
			tmp[1] = 32;
			tmp[2] = 0;
			CALL(registers, tmp);
			break;
		case 0xe8:
			if (registers->flags.p == 1)
				RET(registers);
			break;
		case 0xe9:
			registers->pc = registers->hl;
			break;
		case 0xea:
			if (registers->flags.p == 1)
				JMP(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xeb:
			XCHG(registers);
			break;
		case 0xec:
			if (registers->flags.p == 1)
				CALL(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xee:
			XRA(registers, &opcode[1]);
			registers->pc++;
			break;
		case 0xef:
			tmp[1] = 40;
			tmp[2] = 0;
			CALL(registers, tmp);
			break;
		case 0xf0:
			if (registers->flags.s == 0)
				RET(registers);
			break;
		case 0xf1:
			POP_PSW(registers);
			break;
		case 0xf2:
			if (registers->flags.s == 0)
				JMP(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xf3:
			registers->interrupts = 0;
			break;
		case 0xf4: 
			if (registers->flags.s == 0)
				CALL(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xf5:
			PUSH_PSW(registers);
			break;
		case 0xf6:
			ORA(registers, &opcode[1]);
			registers->pc++;
			break;
		case 0xf7:
			tmp[1] = 48;
			tmp[2] = 0;
			CALL(registers, tmp);
			break;
		case 0xf8:
			if (registers->flags.s == 1)
				RET(registers);
			break;
		case 0xf9:
			registers->sp = registers->hl;
			break;
		case 0xfa:
			if (registers->flags.s == 1)
				CALL(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xfb:
			registers->interrupts = 1;
			break;
		case 0xfc:
			if (registers->flags.s == 1)
				CALL(registers, opcode);
			else
				registers->pc += 2;
			break;
		case 0xfe:
			CMP(registers, &opcode[1]);
			registers->pc++;
			break;
		case 0xff:
			tmp[1] = 56;
			tmp[2] = 0;
			CALL(registers, tmp);
			break;

		default:
			printf("Unknown Instruction: 0x%02x", *opcode);
			break;
    }

    return 0;

}


long LoadROM(Registers *registers, const char *filename, uint16_t offset)
{
    long file_bytes = 0;
    FILE *fp = NULL;
    
    fp = fopen(filename, "rb");
    if (fp == NULL)
        return -1;

    fseek(fp, 0, SEEK_END);
    file_bytes = ftell(fp);
    fseek(fp, 0, SEEK_SET);
	if (file_bytes > 65536)
		return -2;
    fread(registers->memory + offset, file_bytes, 1, fp);
    fclose(fp);

    return file_bytes;
}

void showCPUDebugInfos(Registers *registers)
{
	printf("\n<REG> A : 0x%X (%d)\n", registers->a, registers->a);
	printf("<REG> BC : 0x%X (%d)\t\tB : 0x%X (%d)\t\tC : 0x%X (%d)\n", registers->bc, registers->bc, registers->b, registers->b, registers->c, registers->c);
	printf("<REG> DE : 0x%X (%d)\t\tD : 0x%X (%d)\t\tE : 0x%X (%d)\n", registers->de, registers->de, registers->d, registers->d, registers->e, registers->e);
	printf("<REG> HL : 0x%X (%d)\t\tH : 0x%X (%d)\t\tL : 0x%X (%d)\n", registers->hl, registers->hl, registers->h, registers->h, registers->l, registers->l);
	printf("<REG> SP : 0x%X (%d)\n", registers->sp, registers->sp);
	printf("<REG> PC : 0x%X (%d)\n", registers->pc, registers->pc);
	printf("<FLAG> AC : 0x%X (%d)\tCY : 0x%X (%d)\tP : 0x%X (%d)\tS : 0x%X (%d)\tZ : 0x%X (%d)\n\n", registers->flags.ac, registers->flags.ac, registers->flags.cy,registers->flags.cy, registers->flags.p, registers->flags.p, registers->flags.s, registers->flags.s, registers->flags.z, registers->flags.z);
}