#include <x86/opcode.h>
#include <x86/dasm.h>
#include <stdlib.h>
#include <string.h>

struct x86_Opcode * x86_Opcode_Create(int size){
    struct x86_Opcode * opcodes = malloc(sizeof(struct x86_Opcode) * size);
    memset(opcodes, 0, sizeof(struct x86_Opcode) * size);

    int i=0;
    while(i < size){
        opcodes[i].code = i;
        opcodes[i].callback = x86_InvalidOpcode;
        i++;
    }
    return opcodes;
}

int x86_InvalidOpcode(struct x86_Opcode * opcode, struct x86_CPU * cpu){
    printf("Invalid opcode 0x%X\n", opcode->code);

    char buffer[256];
    memset(buffer, 0, 256);

    unsigned int address = 0;
    if(x86_CPU_GetAddress(cpu, &address, cpu->cs, cpu->eip)) return 1;

    if(x86_DASM_Disassemble(cpu->opcodes, 16, 16, Memory_GetAddress(&cpu->machine->memory, address), buffer, 256)>0){
        printf("%s\n", buffer);
    }else{
        printf("Failed to decode\n");
    }

    return 1;
}


int x86_Mov_A_Imm(struct x86_Opcode * opcode, struct x86_CPU * cpu){
    unsigned short imm = 0;
    if(x86_CPU_LoadByte(cpu, &imm, X86_CS, cpu->run.eip)) return 1;
    cpu->run.eip+=2;
    cpu->ax = imm;
    return 0;
}
int x86_Mov_C_Imm(struct x86_Opcode * opcode, struct x86_CPU * cpu){
    unsigned short imm = 0;
    if(x86_CPU_LoadByte(cpu, &imm, X86_CS, cpu->run.eip)) return 1;
    cpu->run.eip+=2;
    cpu->cx = imm;
    return 0;
}
int x86_Mov_D_Imm(struct x86_Opcode * opcode, struct x86_CPU * cpu){
    unsigned short imm = 0;
    if(x86_CPU_LoadByte(cpu, &imm, X86_CS, cpu->run.eip)) return 1;
    cpu->run.eip+=2;
    cpu->dx = imm;
    return 0;
}
int x86_Mov_B_Imm(struct x86_Opcode * opcode, struct x86_CPU * cpu){
    unsigned short imm = 0;
    if(x86_CPU_LoadByte(cpu, &imm, X86_CS, cpu->run.eip)) return 1;
    cpu->run.eip+=2;
    cpu->bx = imm;
    return 0;
}
int x86_Mov_SP_Imm(struct x86_Opcode * opcode, struct x86_CPU * cpu){
    unsigned short imm = 0;
    if(x86_CPU_LoadByte(cpu, &imm, X86_CS, cpu->run.eip)) return 1;
    cpu->run.eip+=2;
    cpu->sp = imm;
    return 0;
}
int x86_Mov_BP_Imm(struct x86_Opcode * opcode, struct x86_CPU * cpu){
    unsigned short imm = 0;
    if(x86_CPU_LoadByte(cpu, &imm, X86_CS, cpu->run.eip)) return 1;
    cpu->run.eip+=2;
    cpu->bp = imm;
    return 0;
}
int x86_Mov_SI_Imm(struct x86_Opcode * opcode, struct x86_CPU * cpu){
    unsigned short imm = 0;
    if(x86_CPU_LoadByte(cpu, &imm, X86_CS, cpu->run.eip)) return 1;
    cpu->run.eip+=2;
    cpu->si = imm;
    return 0;
}
int x86_Mov_DI_Imm(struct x86_Opcode * opcode, struct x86_CPU * cpu){
    unsigned short imm = 0;
    if(x86_CPU_LoadByte(cpu, &imm, X86_CS, cpu->run.eip)) return 1;
    cpu->run.eip+=2;
    cpu->di = imm;
    return 0;
}
