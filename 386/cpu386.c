#include "386/cpu386.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

inline int CPU386_Execute(struct CPU386 * cpu);

union ModRm {
    struct {
        unsigned char rm : 3;
        unsigned char reg : 3;
        unsigned char mod : 2;
    };
    unsigned char value;
};

void CPU386_Run(struct CPU386 * cpu){
    int error = 0;
    while(1){
        // if interrupts enabled and has interrupts, run interrupt
        if((error = CPU386_Execute(cpu))!=0) break;
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

void CPU386_SetInt(struct CPU386 * cpu, unsigned char mode, unsigned char num, int (*callback)(struct CPU386 * cpu)){
    cpu->ints[mode][num] = malloc(sizeof(struct Interrupt386));
    memset(cpu->ints[mode][num], 0, sizeof(struct Interrupt386));
    cpu->ints[mode][num]->callback = callback;
}

void CPU386_Attach(struct Machine * machine){
    struct CPU386 * cpu = malloc(sizeof(struct CPU386));
    memset(cpu, 0, sizeof(struct CPU386));
    cpu->cpu.run = (void*)CPU386_Run;
    cpu->cpu.setint = (void*)CPU386_SetInt;
    cpu->machine = machine;

    machine->cpu = (void*)cpu;
}




inline void CPU386_PrintModRm(union ModRm modrm){
    printf("ModRm: 0x%02X\n", modrm.value);
    printf(" mod: 0x%X\n", modrm.mod);
    printf(" reg: 0x%X\n", modrm.reg);
    printf(" rm : 0x%X\n", modrm.rm);
}

inline unsigned short CPU386_ReadByte(struct CPU386 * cpu, unsigned char * code){
    return code[cpu->ip++];
}
inline unsigned short CPU386_ReadWord(struct CPU386 * cpu, unsigned char * code){
    short value = *((unsigned short*)(&code[cpu->ip]));
    cpu->ip+= 2;
    return value;
}
inline unsigned short CPU386_ReadDword(struct CPU386 * cpu, unsigned char * code){
    short value = *((unsigned short*)(&code[cpu->ip]));
    cpu->ip+= 4;
    return value;
}

inline int CPU386_16_GetRegb(struct CPU386 * cpu, int reg, unsigned char * value){
    switch(reg){
        case 0: *value = cpu->al; return 0;
        case 1: *value = cpu->cl; return 0;
        case 2: *value = cpu->dl; return 0;
        case 3: *value = cpu->bl; return 0;
        case 4: *value = cpu->ah; return 0;
        case 5: *value = cpu->ch; return 0;
        case 6: *value = cpu->dh; return 0;
        case 7: *value = cpu->bh; return 0;
    }
    return 1;
}

inline int CPU386_16_GetRegw(struct CPU386 * cpu, int reg, unsigned short * value){
    switch(reg){
        case 0: *value = cpu->ax; return 0;
        case 1: *value = cpu->cx; return 0;
        case 2: *value = cpu->dx; return 0;
        case 3: *value = cpu->bx; return 0;
        case 4: *value = cpu->sp; return 0;
        case 5: *value = cpu->bp; return 0;
        case 6: *value = cpu->si; return 0;
        case 7: *value = cpu->di; return 0;
    }
    return 1;
}

inline void CPU386_16_AjustFlagsForByte(struct CPU386 * cpu, unsigned char value){
    cpu->pf = value&1 ? 0 : 1;
    cpu->zf = value==0 ? 1 : 0;
    cpu->sf = value&128 ? 1 : 0;
}

inline int CPU386_16_InvalidOpcodde(unsigned char opcode){ printf("Invalid opcode 0x%02X\n", opcode); return 1; }

inline int CPU386_16_Orb_ModRm(struct CPU386 * cpu, unsigned char * code){
    union ModRm modrm;
    modrm.value = CPU386_ReadByte(cpu, code);

    switch(modrm.mod){
        case 3:{
            unsigned char value;
            if(CPU386_16_GetRegb(cpu, modrm.reg, &value)!=0) return 1;

            switch(modrm.rm){
                case 0: cpu->al|= value; CPU386_16_AjustFlagsForByte(cpu, cpu->al); break;
                case 1: cpu->cl|= value; CPU386_16_AjustFlagsForByte(cpu, cpu->cl); break;
                case 2: cpu->dl|= value; CPU386_16_AjustFlagsForByte(cpu, cpu->dl); break;
                case 3: cpu->bl|= value; CPU386_16_AjustFlagsForByte(cpu, cpu->bl); break;
                case 4: cpu->ah|= value; CPU386_16_AjustFlagsForByte(cpu, cpu->ah); break;
                case 5: cpu->ch|= value; CPU386_16_AjustFlagsForByte(cpu, cpu->ch); break;
                case 6: cpu->dh|= value; CPU386_16_AjustFlagsForByte(cpu, cpu->dh); break;
                case 7: cpu->bh|= value; CPU386_16_AjustFlagsForByte(cpu, cpu->bh); break;
            }
            return 0;
        } break;
    }

    return 1;
}

inline int CPU386_16_Lodsb(struct CPU386 * cpu){
    unsigned char * data = Memory_GetAddress(&cpu->machine->memory, cpu->ds << 4);
    cpu->al = data[cpu->si];
    if(cpu->df){
        cpu->si-=1;
    }else{
        cpu->si+=1;
    }
    return 0;
}

inline int CPU386_16_Lodsw(struct CPU386 * cpu){
    unsigned char * data = Memory_GetAddress(&cpu->machine->memory, cpu->ds << 4);
    cpu->ax = *((unsigned short *)(data + cpu->si));
    if(cpu->df){
        cpu->si-=2;
    }else{
        cpu->si+=2;
    }
    return 0;
}

inline int CPU386_16_Jz_Immb(struct CPU386 * cpu, unsigned char * code){
   int value = (char)CPU386_ReadByte(cpu, code);
   if(cpu->zf) cpu->ip+= value;
   return 0;
}

inline int CPU386_16_Jmp_Immb(struct CPU386 * cpu, unsigned char * code){
   int value = (char)CPU386_ReadByte(cpu, code);
   cpu->ip+= value;
   return 0;
}

inline int CPU386_16_Move_XS_ModRm(struct CPU386 * cpu, unsigned char * code){
    union ModRm modrm;
    modrm.value = CPU386_ReadByte(cpu, code);
    if(modrm.mod!=3) {
        printf("Invalid mode");
        return 1;
    }
    switch(modrm.reg){
        case 0: return CPU386_16_GetRegw(cpu, modrm.rm, &cpu->es);
        case 1: return CPU386_16_GetRegw(cpu, modrm.rm, &cpu->cs);
        case 2: return CPU386_16_GetRegw(cpu, modrm.rm, &cpu->ss);
        case 3: return CPU386_16_GetRegw(cpu, modrm.rm, &cpu->ds);
        case 4: return CPU386_16_GetRegw(cpu, modrm.rm, &cpu->fs);
        case 5: return CPU386_16_GetRegw(cpu, modrm.rm, &cpu->gs);
        default:
            printf("Invalid segment register %d\n", modrm.reg);
        return 1;
    }
    return 0;
}

inline int CPU386_16_Move_AL_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->al = CPU386_ReadByte(cpu, code); return 0; }
inline int CPU386_16_Move_CL_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->cl = CPU386_ReadByte(cpu, code); return 0; }
inline int CPU386_16_Move_DL_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->dl = CPU386_ReadByte(cpu, code); return 0; }
inline int CPU386_16_Move_BL_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->bl = CPU386_ReadByte(cpu, code); return 0; }
inline int CPU386_16_Move_AH_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->ah = CPU386_ReadByte(cpu, code); return 0; }
inline int CPU386_16_Move_CH_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->ch = CPU386_ReadByte(cpu, code); return 0; }
inline int CPU386_16_Move_DH_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->dh = CPU386_ReadByte(cpu, code); return 0; }
inline int CPU386_16_Move_BH_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->bh = CPU386_ReadByte(cpu, code); return 0; }

inline int CPU386_16_Move_AX_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->ax = CPU386_ReadWord(cpu, code); return 0; }
inline int CPU386_16_Move_CX_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->cx = CPU386_ReadWord(cpu, code); return 0; }
inline int CPU386_16_Move_DX_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->dx = CPU386_ReadWord(cpu, code); return 0; }
inline int CPU386_16_Move_BX_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->bx = CPU386_ReadWord(cpu, code); return 0; }
inline int CPU386_16_Move_SP_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->sp = CPU386_ReadWord(cpu, code); return 0; }
inline int CPU386_16_Move_BP_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->bp = CPU386_ReadWord(cpu, code); return 0; }
inline int CPU386_16_Move_SI_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->si = CPU386_ReadWord(cpu, code); return 0; }
inline int CPU386_16_Move_DI_Imm(struct CPU386 * cpu, unsigned char * code){ cpu->di = CPU386_ReadWord(cpu, code); return 0; }


inline int CPU386_16_Interrupt(struct CPU386 * cpu, unsigned char * code){
    unsigned char num = CPU386_ReadByte(cpu, code);
    if(cpu->ints[0][num]){
        if(cpu->ints[0][num]->callback) return cpu->ints[0][num]->callback(cpu);
    }
    printf("Unhandeled interrupt 0x%02X\n", num);
    return 1;
}

inline int CPU386_Execute(struct CPU386 * cpu){
    unsigned char * code = Memory_GetAddress(&cpu->machine->memory, cpu->cs << 4);
    unsigned char opcode = CPU386_ReadByte(cpu, code);
    switch(opcode){
        case 0x00: return CPU386_16_InvalidOpcodde(opcode); // add modrm, reg       | 8bit
        case 0x01: return CPU386_16_InvalidOpcodde(opcode); // add modrm, reg       | 16/32bit
        case 0x02: return CPU386_16_InvalidOpcodde(opcode); // add reg, modrm       | 8bit
        case 0x03: return CPU386_16_InvalidOpcodde(opcode); // add reg, modrm       | 16/32bit
        case 0x04: return CPU386_16_InvalidOpcodde(opcode); // add a, imm           | 8bit
        case 0x05: return CPU386_16_InvalidOpcodde(opcode); // add a, imm           | 16/32bit
        case 0x06: return CPU386_16_InvalidOpcodde(opcode); // push es
        case 0x07: return CPU386_16_InvalidOpcodde(opcode); // pop es
        case 0x08: return CPU386_16_Orb_ModRm(cpu, code);   // or modrm, reg        | 8bit
        case 0x09: return CPU386_16_InvalidOpcodde(opcode); // or modrm, reg        | 16/32bit
        case 0x0A: return CPU386_16_InvalidOpcodde(opcode); // or reg, modrm        | 8bit
        case 0x0B: return CPU386_16_InvalidOpcodde(opcode); // or reg, modrm        | 16/32bit
        case 0x0C: return CPU386_16_InvalidOpcodde(opcode); // or a, imm            | 8bit
        case 0x0D: return CPU386_16_InvalidOpcodde(opcode); // or a, imm            | 16/32bit
        case 0x0E: return CPU386_16_InvalidOpcodde(opcode); // push cs
        case 0x0F: return CPU386_16_InvalidOpcodde(opcode); // subtable 0x0F

        case 0x10: return CPU386_16_InvalidOpcodde(opcode); // adc modrm, reg       | 8bit
        case 0x11: return CPU386_16_InvalidOpcodde(opcode); // adc modrm, reg       | 16/32bit
        case 0x12: return CPU386_16_InvalidOpcodde(opcode); // adc reg, modrm       | 8bit
        case 0x13: return CPU386_16_InvalidOpcodde(opcode); // adc reg, modrm       | 16/32bit
        case 0x14: return CPU386_16_InvalidOpcodde(opcode); // adc a, imm           | 8bit
        case 0x15: return CPU386_16_InvalidOpcodde(opcode); // adc a, imm           | 16/32bit
        case 0x16: return CPU386_16_InvalidOpcodde(opcode); // push ss
        case 0x17: return CPU386_16_InvalidOpcodde(opcode); // pop ss
        case 0x18: return CPU386_16_InvalidOpcodde(opcode); // sbb modrm, reg       | 8bit
        case 0x19: return CPU386_16_InvalidOpcodde(opcode); // sbb modrm, reg       | 16/32bit
        case 0x1A: return CPU386_16_InvalidOpcodde(opcode); // sbb reg, modrm       | 8bit
        case 0x1B: return CPU386_16_InvalidOpcodde(opcode); // sbb reg, modrm       | 16/32bit
        case 0x1C: return CPU386_16_InvalidOpcodde(opcode); // sbb a, imm           | 8bit
        case 0x1D: return CPU386_16_InvalidOpcodde(opcode); // sbb a, imm           | 16/32bit
        case 0x1E: return CPU386_16_InvalidOpcodde(opcode); // push ds
        case 0x1F: return CPU386_16_InvalidOpcodde(opcode); // pop ds

        case 0x20: return CPU386_16_InvalidOpcodde(opcode); // and modrm, reg       | 8bit
        case 0x21: return CPU386_16_InvalidOpcodde(opcode); // and modrm, reg       | 16/32bit
        case 0x22: return CPU386_16_InvalidOpcodde(opcode); // and reg, modrm       | 8bit
        case 0x23: return CPU386_16_InvalidOpcodde(opcode); // and reg, modrm       | 16/32bit
        case 0x24: return CPU386_16_InvalidOpcodde(opcode); // and a, imm           | 8bit
        case 0x25: return CPU386_16_InvalidOpcodde(opcode); // and a, imm           | 16/32bit
        case 0x26: return CPU386_16_InvalidOpcodde(opcode); // es segment override
        case 0x27: return CPU386_16_InvalidOpcodde(opcode); // daa
        case 0x28: return CPU386_16_InvalidOpcodde(opcode); // sub modrm, reg       | 8bit
        case 0x29: return CPU386_16_InvalidOpcodde(opcode); // sub modrm, reg       | 16/32bit
        case 0x2A: return CPU386_16_InvalidOpcodde(opcode); // sub reg, modrm       | 8bit
        case 0x2B: return CPU386_16_InvalidOpcodde(opcode); // sub reg, modrm       | 16/32bit
        case 0x2C: return CPU386_16_InvalidOpcodde(opcode); // sub a, imm           | 8bit
        case 0x2D: return CPU386_16_InvalidOpcodde(opcode); // sub a, imm           | 16/32bit
        case 0x2E: return CPU386_16_InvalidOpcodde(opcode); // cs segment override
        case 0x2F: return CPU386_16_InvalidOpcodde(opcode); // das

        case 0x30: return CPU386_16_InvalidOpcodde(opcode); // xor modrm, reg       | 8bit
        case 0x31: return CPU386_16_InvalidOpcodde(opcode); // xor modrm, reg       | 16/32bit
        case 0x32: return CPU386_16_InvalidOpcodde(opcode); // xor reg, modrm       | 8bit
        case 0x33: return CPU386_16_InvalidOpcodde(opcode); // xor reg, modrm       | 16/32bit
        case 0x34: return CPU386_16_InvalidOpcodde(opcode); // xor a, imm           | 8bit
        case 0x35: return CPU386_16_InvalidOpcodde(opcode); // xor a, imm           | 16/32bit
        case 0x36: return CPU386_16_InvalidOpcodde(opcode); // ss segment override
        case 0x37: return CPU386_16_InvalidOpcodde(opcode); // aaa
        case 0x38: return CPU386_16_InvalidOpcodde(opcode); // cmp modrm, reg       | 8bit
        case 0x39: return CPU386_16_InvalidOpcodde(opcode); // cmp modrm, reg       | 16/32bit
        case 0x3A: return CPU386_16_InvalidOpcodde(opcode); // cmp reg, modrm       | 8bit
        case 0x3B: return CPU386_16_InvalidOpcodde(opcode); // cmp reg, modrm       | 16/32bit
        case 0x3C: return CPU386_16_InvalidOpcodde(opcode); // cmp a, imm           | 8bit
        case 0x3D: return CPU386_16_InvalidOpcodde(opcode); // cmp a, imm           | 16/32bit
        case 0x3E: return CPU386_16_InvalidOpcodde(opcode); // ds segment override
        case 0x3F: return CPU386_16_InvalidOpcodde(opcode); // aas

        case 0x40: return CPU386_16_InvalidOpcodde(opcode); // inc A                | 16/32bit
        case 0x41: return CPU386_16_InvalidOpcodde(opcode); // inc C                | 16/32bit
        case 0x42: return CPU386_16_InvalidOpcodde(opcode); // inc D                | 16/32bit
        case 0x43: return CPU386_16_InvalidOpcodde(opcode); // inc B                | 16/32bit
        case 0x44: return CPU386_16_InvalidOpcodde(opcode); // inc SP               | 16/32bit
        case 0x45: return CPU386_16_InvalidOpcodde(opcode); // inc BP               | 16/32bit
        case 0x46: return CPU386_16_InvalidOpcodde(opcode); // inc SI               | 16/32bit
        case 0x47: return CPU386_16_InvalidOpcodde(opcode); // inc DI               | 16/32bit
        case 0x48: return CPU386_16_InvalidOpcodde(opcode); // dec A                | 16/32bit
        case 0x49: return CPU386_16_InvalidOpcodde(opcode); // dec C                | 16/32bit
        case 0x4A: return CPU386_16_InvalidOpcodde(opcode); // dec D                | 16/32bit
        case 0x4B: return CPU386_16_InvalidOpcodde(opcode); // dec B                | 16/32bit
        case 0x4C: return CPU386_16_InvalidOpcodde(opcode); // dec SP               | 16/32bit
        case 0x4D: return CPU386_16_InvalidOpcodde(opcode); // dec BP               | 16/32bit
        case 0x4E: return CPU386_16_InvalidOpcodde(opcode); // dec SI               | 16/32bit
        case 0x4F: return CPU386_16_InvalidOpcodde(opcode); // dec DI               | 16/32bit

        case 0x50: return CPU386_16_InvalidOpcodde(opcode); // push A               | 16/32bit
        case 0x51: return CPU386_16_InvalidOpcodde(opcode); // push C               | 16/32bit
        case 0x52: return CPU386_16_InvalidOpcodde(opcode); // push D               | 16/32bit
        case 0x53: return CPU386_16_InvalidOpcodde(opcode); // push B               | 16/32bit
        case 0x54: return CPU386_16_InvalidOpcodde(opcode); // push SP              | 16/32bit
        case 0x55: return CPU386_16_InvalidOpcodde(opcode); // push BP              | 16/32bit
        case 0x56: return CPU386_16_InvalidOpcodde(opcode); // push SI              | 16/32bit
        case 0x57: return CPU386_16_InvalidOpcodde(opcode); // push DI              | 16/32bit
        case 0x58: return CPU386_16_InvalidOpcodde(opcode); // pop A                | 16/32bit
        case 0x59: return CPU386_16_InvalidOpcodde(opcode); // pop C                | 16/32bit
        case 0x5A: return CPU386_16_InvalidOpcodde(opcode); // pop D                | 16/32bit
        case 0x5B: return CPU386_16_InvalidOpcodde(opcode); // pop B                | 16/32bit
        case 0x5C: return CPU386_16_InvalidOpcodde(opcode); // pop SP               | 16/32bit
        case 0x5D: return CPU386_16_InvalidOpcodde(opcode); // pop BP               | 16/32bit
        case 0x5E: return CPU386_16_InvalidOpcodde(opcode); // pop SI               | 16/32bit
        case 0x5F: return CPU386_16_InvalidOpcodde(opcode); // pop DI               | 16/32bit

        case 0x60: return CPU386_16_InvalidOpcodde(opcode); // pusha                | 16/32bit
        case 0x61: return CPU386_16_InvalidOpcodde(opcode); // popa                 | 16/32bit
        case 0x62: return CPU386_16_InvalidOpcodde(opcode); // bound reg, modrm     | ?bit
        case 0x63: return CPU386_16_InvalidOpcodde(opcode); // arpl reg, modrm      | 16bit/pmode
        case 0x64: return CPU386_16_InvalidOpcodde(opcode); // fs segment override
        case 0x65: return CPU386_16_InvalidOpcodde(opcode); // gs segment override
        case 0x66: return CPU386_16_InvalidOpcodde(opcode); // flip 16/32 but mode
        case 0x67: return CPU386_16_InvalidOpcodde(opcode); // address prefix????
        case 0x68: return CPU386_16_InvalidOpcodde(opcode); // push imm             | 16/32bit
        case 0x69: return CPU386_16_InvalidOpcodde(opcode); // imul reg, modrm, imm | 16/32bit
        case 0x6A: return CPU386_16_InvalidOpcodde(opcode); // push imm             | 8bit
        case 0x6B: return CPU386_16_InvalidOpcodde(opcode); // imul reg, modrm, imm | 8bit
        case 0x6C: return CPU386_16_InvalidOpcodde(opcode); // insb                 | 8bit
        case 0x6D: return CPU386_16_InvalidOpcodde(opcode); // ins                  | 16/32bit
        case 0x6E: return CPU386_16_InvalidOpcodde(opcode); // outsb                | 8bit
        case 0x6F: return CPU386_16_InvalidOpcodde(opcode); // outs                 | 16/32bit

        case 0x70: return CPU386_16_InvalidOpcodde(opcode); // jo imm
        case 0x71: return CPU386_16_InvalidOpcodde(opcode); // jno imm
        case 0x72: return CPU386_16_InvalidOpcodde(opcode); // jc imm
        case 0x73: return CPU386_16_InvalidOpcodde(opcode); // jnc imm
        case 0x74: return CPU386_16_Jz_Immb(cpu, code);     // jz imm
        case 0x75: return CPU386_16_InvalidOpcodde(opcode); // jnz imm
        case 0x76: return CPU386_16_InvalidOpcodde(opcode); // jbe imm
        case 0x77: return CPU386_16_InvalidOpcodde(opcode); // ja imm
        case 0x78: return CPU386_16_InvalidOpcodde(opcode); // js imm
        case 0x79: return CPU386_16_InvalidOpcodde(opcode); // jns imm
        case 0x7A: return CPU386_16_InvalidOpcodde(opcode); // jp imm
        case 0x7B: return CPU386_16_InvalidOpcodde(opcode); // jnp imm
        case 0x7C: return CPU386_16_InvalidOpcodde(opcode); // jl imm
        case 0x7D: return CPU386_16_InvalidOpcodde(opcode); // jge imm
        case 0x7E: return CPU386_16_InvalidOpcodde(opcode); // jle imm
        case 0x7F: return CPU386_16_InvalidOpcodde(opcode); // jg imm

        case 0x80: return CPU386_16_InvalidOpcodde(opcode); // subtable 0x80
        case 0x81: return CPU386_16_InvalidOpcodde(opcode); // subtable 0x81
        case 0x82: return CPU386_16_InvalidOpcodde(opcode); // subtable 0x82
        case 0x83: return CPU386_16_InvalidOpcodde(opcode); // subtable 0x83
        case 0x84: return CPU386_16_InvalidOpcodde(opcode); // test modrm, reg      | 8bit
        case 0x85: return CPU386_16_InvalidOpcodde(opcode); // test modrm, reg      | 16/32bit
        case 0x86: return CPU386_16_InvalidOpcodde(opcode); // xchg modrm, reg      | 8bit
        case 0x87: return CPU386_16_InvalidOpcodde(opcode); // xchg modrm, reg      | 16/32bit
        case 0x88: return CPU386_16_InvalidOpcodde(opcode); // mov modrm, reg       | 8bit
        case 0x89: return CPU386_16_InvalidOpcodde(opcode); // mov modrm, reg       | 16/32bit
        case 0x8A: return CPU386_16_InvalidOpcodde(opcode); // mov reg, modrm       | 8bit
        case 0x8B: return CPU386_16_InvalidOpcodde(opcode); // mov reg, modrm       | 16/32bit
        case 0x8C: return CPU386_16_InvalidOpcodde(opcode); // mov modrm, segment
        case 0x8D: return CPU386_16_InvalidOpcodde(opcode); // lea reg, modrm
        case 0x8E: return CPU386_16_Move_XS_ModRm(cpu, code); // mov segment, modrm
        case 0x8F: return CPU386_16_InvalidOpcodde(opcode); // pop modrm            | 16/32bit

        case 0x90: return CPU386_16_InvalidOpcodde(opcode); // nop
        case 0x91: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x92: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x93: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x94: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x95: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x96: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x97: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x98: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x99: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x9A: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x9B: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x9C: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x9D: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x9E: return CPU386_16_InvalidOpcodde(opcode); //
        case 0x9F: return CPU386_16_InvalidOpcodde(opcode); //

        case 0xA0: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xA1: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xA2: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xA3: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xA4: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xA5: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xA6: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xA7: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xA8: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xA9: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xAA: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xAB: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xAC: return CPU386_16_Lodsb(cpu);             //
        case 0xAD: return CPU386_16_Lodsw(cpu);             //
        case 0xAE: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xAF: return CPU386_16_InvalidOpcodde(opcode); //

        case 0xB0: return CPU386_16_Move_AL_Imm(cpu, code); //
        case 0xB1: return CPU386_16_Move_CL_Imm(cpu, code); //
        case 0xB2: return CPU386_16_Move_DL_Imm(cpu, code); //
        case 0xB3: return CPU386_16_Move_BL_Imm(cpu, code); //
        case 0xB4: return CPU386_16_Move_AH_Imm(cpu, code); //
        case 0xB5: return CPU386_16_Move_CH_Imm(cpu, code); //
        case 0xB6: return CPU386_16_Move_DH_Imm(cpu, code); //
        case 0xB7: return CPU386_16_Move_BH_Imm(cpu, code); //
        case 0xB8: return CPU386_16_Move_AX_Imm(cpu, code); //
        case 0xB9: return CPU386_16_Move_CX_Imm(cpu, code); //
        case 0xBA: return CPU386_16_Move_DX_Imm(cpu, code); //
        case 0xBB: return CPU386_16_Move_BX_Imm(cpu, code); //
        case 0xBC: return CPU386_16_Move_SP_Imm(cpu, code); //
        case 0xBD: return CPU386_16_Move_BP_Imm(cpu, code); //
        case 0xBE: return CPU386_16_Move_SI_Imm(cpu, code); //
        case 0xBF: return CPU386_16_Move_DI_Imm(cpu, code); //

        case 0xC0: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xC1: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xC2: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xC3: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xC4: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xC5: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xC6: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xC7: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xC8: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xC9: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xCA: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xCB: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xCC: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xCD: return CPU386_16_Interrupt(cpu, code);   //
        case 0xCE: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xCF: return CPU386_16_InvalidOpcodde(opcode); //

        case 0xD0: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xD1: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xD2: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xD3: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xD4: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xD5: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xD6: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xD7: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xD8: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xD9: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xDA: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xDB: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xDC: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xDD: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xDE: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xDF: return CPU386_16_InvalidOpcodde(opcode); //

        case 0xE0: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xE1: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xE2: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xE3: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xE4: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xE5: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xE6: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xE7: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xE8: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xE9: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xEA: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xEB: return CPU386_16_Jmp_Immb(cpu, code);    //
        case 0xEC: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xED: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xEE: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xEF: return CPU386_16_InvalidOpcodde(opcode); //

        case 0xF0: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xF1: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xF2: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xF3: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xF4: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xF5: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xF6: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xF7: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xF8: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xF9: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xFA: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xFB: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xFC: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xFD: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xFE: return CPU386_16_InvalidOpcodde(opcode); //
        case 0xFF: return CPU386_16_InvalidOpcodde(opcode); //
    }

    printf("Unknown opcode 0x%02X\n", opcode);
    return 1;
}
