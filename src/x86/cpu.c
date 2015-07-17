#include <x86/cpu.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "opcode.c"

inline int x86_CPU_GetAddress(struct x86_CPU * cpu, unsigned int * address, unsigned short segment, unsigned int offset){
    *address = (segment << 4) + offset;
    return 0;
}

inline int x86_CPU_LoadByte(struct x86_CPU * cpu, unsigned char * value, unsigned char sreg, unsigned int offset){
    unsigned int address = 0;
    unsigned short segment;

    switch(sreg){
        case X86_CS: segment = cpu->cs; break;
        case X86_SS: segment = cpu->ss; break;
        case X86_DS: segment = cpu->ds; break;
        case X86_ES: segment = cpu->es; break;
        case X86_FS: segment = cpu->fs; break;
        case X86_GS: segment = cpu->gs; break;
        default: return 1;
    }

    if(x86_CPU_GetAddress(cpu, &address, segment, offset)) return 1;

    *value = *((unsigned char *)Memory_GetAddress(&cpu->machine->memory, address));
    return 0;
}

inline int x86_CPU_LoadWord(struct x86_CPU * cpu, unsigned short * value, unsigned char sreg, unsigned int offset){
    unsigned int address = 0;
    unsigned short segment;

    switch(sreg){
        case X86_CS: segment = cpu->cs; break;
        case X86_SS: segment = cpu->ss; break;
        case X86_DS: segment = cpu->ds; break;
        case X86_ES: segment = cpu->es; break;
        case X86_FS: segment = cpu->fs; break;
        case X86_GS: segment = cpu->gs; break;
        default: return 1;
    }

    if(x86_CPU_GetAddress(cpu, &address, segment, offset)) return 1;

    *value = *((unsigned short *)Memory_GetAddress(&cpu->machine->memory, address));
    return 0;
}

inline int x86_CPU_LoadDword(struct x86_CPU * cpu, unsigned int * value, unsigned char sreg, unsigned int offset){
    unsigned int address = 0;
    unsigned short segment;

    switch(sreg){
        case X86_CS: segment = cpu->cs; break;
        case X86_SS: segment = cpu->ss; break;
        case X86_DS: segment = cpu->ds; break;
        case X86_ES: segment = cpu->es; break;
        case X86_FS: segment = cpu->fs; break;
        case X86_GS: segment = cpu->gs; break;
        default: return 1;
    }

    if(x86_CPU_GetAddress(cpu, &address, segment, offset)) return 1;

    *value = *((unsigned int *)Memory_GetAddress(&cpu->machine->memory, address));
    return 0;
}

inline int x86_CPU_StoreByte(struct x86_CPU * cpu, unsigned char value, unsigned char sreg, unsigned int offset){
    unsigned int address = 0;
    unsigned short segment;

    switch(sreg){
        case X86_CS: segment = cpu->cs; break;
        case X86_SS: segment = cpu->ss; break;
        case X86_DS: segment = cpu->ds; break;
        case X86_ES: segment = cpu->es; break;
        case X86_FS: segment = cpu->fs; break;
        case X86_GS: segment = cpu->gs; break;
        default: return 1;
    }

    if(x86_CPU_GetAddress(cpu, &address, segment, offset)) return 1;

    *((unsigned char *)Memory_GetAddress(&cpu->machine->memory, address)) = value;
    return 0;
}

inline int x86_CPU_StoreWord(struct x86_CPU * cpu, unsigned short value, unsigned char sreg, unsigned int offset){
    unsigned int address = 0;
    unsigned short segment;

    switch(sreg){
        case X86_CS: segment = cpu->cs; break;
        case X86_SS: segment = cpu->ss; break;
        case X86_DS: segment = cpu->ds; break;
        case X86_ES: segment = cpu->es; break;
        case X86_FS: segment = cpu->fs; break;
        case X86_GS: segment = cpu->gs; break;
        default: return 1;
    }

    if(x86_CPU_GetAddress(cpu, &address, segment, offset)) return 1;

    *((unsigned short *)Memory_GetAddress(&cpu->machine->memory, address)) = value;
    return 0;
}

inline int x86_CPU_StoreDword(struct x86_CPU * cpu, unsigned int value, unsigned char sreg, unsigned int offset){
    unsigned int address = 0;
    unsigned short segment;

    switch(sreg){
        case X86_CS: segment = cpu->cs; break;
        case X86_SS: segment = cpu->ss; break;
        case X86_DS: segment = cpu->ds; break;
        case X86_ES: segment = cpu->es; break;
        case X86_FS: segment = cpu->fs; break;
        case X86_GS: segment = cpu->gs; break;
        default: return 1;
    }

    if(x86_CPU_GetAddress(cpu, &address, segment, offset)) return 1;

    *((unsigned int *)Memory_GetAddress(&cpu->machine->memory, address)) = value;
    return 0;
}

int x86_CPU_Execute(struct x86_CPU * cpu){
    unsigned char opcode = 0x90;
    if(x86_CPU_LoadByte(cpu, &opcode, X86_CS, cpu->run.eip++)) return 1;

    return cpu->opcodes[opcode].callback(&cpu->opcodes[opcode], cpu);
}

void x86_CPU_Run(struct x86_CPU * cpu){
    int error = 0;
    while(1){
        // if interrupts enabled and has interrupts, run interrupt
        cpu->run.eip = cpu->eip;
        if((error = x86_CPU_Execute(cpu))!=0) break;
        cpu->eip = cpu->run.eip;
    }

    if(error){
        printf("EAX: %08X\n", cpu->eax);
        printf("ECX: %08X\n", cpu->ecx);
        printf("EDX: %08X\n", cpu->edx);
        printf("EBX: %08X\n", cpu->ebx);
        printf("ESP: %08X\n", cpu->esp);
        printf("EBP: %08X\n", cpu->ebp);
        printf("ESI: %08X\n", cpu->esi);
        printf("EDI: %08X\n", cpu->edi);
        printf("EIP: %08X\n", cpu->eip);

        printf("ES : %04X\n", cpu->es);
        printf("CS : %04X\n", cpu->cs);
        printf("SS : %04X\n", cpu->ss);
        printf("DS : %04X\n", cpu->ds);
        printf("FS : %04X\n", cpu->fs);
        printf("GS : %04X\n", cpu->gs);


        printf("CF ?? PF ?? AF ?? ZF SF TF IF DF OF IO NT ?? RF AC VIF VIP ID\n");
        printf("%d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d   %d   %d  %d  \n"
               , cpu->cf, cpu->unused1, cpu->pf, cpu->unused2, cpu->af, cpu->unused3, cpu->zf, cpu->sf, cpu->tf, cpu->ief
               , cpu->df, cpu->of, cpu->io, cpu->nt, cpu->unused4, cpu->rf, cpu->ac , cpu->vif , cpu->vip , cpu->id);
    }
}

void x86_CPU_SetInterrupt(struct x86_CPU * cpu, unsigned char mode, unsigned char num, int (*callback)(struct x86_CPU * cpu)){
    cpu->interrupts[mode][num] = malloc(sizeof(struct x86_Interrupt));
    memset(cpu->interrupts[mode][num], 0, sizeof(struct x86_Interrupt));
    cpu->interrupts[mode][num]->callback = callback;
}

void x86_CPU_Attach(struct Machine * machine){
    struct x86_CPU * cpu = malloc(sizeof(struct x86_CPU));
    memset(cpu, 0, sizeof(struct x86_CPU));
    cpu->cpu.run = (void*)x86_CPU_Run;
    cpu->machine = machine;
    machine->cpu = (void*)cpu;

    cpu->opcodes = x86_Opcode_Create(256);
    x86_DASM_Load(cpu->opcodes, "opcodes.info");

    cpu->opcodes[0xB8].callback = x86_Mov_A_Imm;
}
