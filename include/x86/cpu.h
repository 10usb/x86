#ifndef X86_CPU_H
#define X86_CPU_H

#include <cpu.h>
#include <machine.h>
#include <x86/interrupt.h>
#include <x86/opcode.h>

struct x86_CPU {
    struct CPU cpu;
    struct Machine * machine;
    union { struct { unsigned char  al; unsigned char ah; }; unsigned short ax; unsigned int eax; };
    union { struct { unsigned char  cl; unsigned char ch; }; unsigned short cx; unsigned int ecx; };
    union { struct { unsigned char  dl; unsigned char dh; }; unsigned short dx; unsigned int edx; };
    union { struct { unsigned char  bl; unsigned char bh; }; unsigned short bx; unsigned int ebx; };
    union { unsigned short sp; unsigned int esp; };
    union { unsigned short bp; unsigned int ebp; };
    union { unsigned short si; unsigned int esi; };
    union { unsigned short di; unsigned int edi; };
    unsigned short cs;
    unsigned short ss;
    unsigned short ds;
    unsigned short es;
    unsigned short fs;
    unsigned short gs;
    unsigned int eip;
    struct {
        unsigned int eip;
        unsigned int operandsize;
        unsigned int addresssize;
    } run;
    union {
        unsigned short flags;
        unsigned int eflags;
        struct {
            unsigned cf : 1;
            unsigned unused1 : 1;
            unsigned pf : 1;
            unsigned unused2 : 1;
            unsigned af : 1;
            unsigned unused3 : 1;
            unsigned zf : 1;
            unsigned sf : 1;
            unsigned tf : 1;
            unsigned ief : 1;
            unsigned df : 1;
            unsigned of : 1;
            unsigned io : 2;
            unsigned nt : 1;
            unsigned unused4 : 1;
            unsigned rf : 1;
            unsigned ac : 1;
            unsigned vif : 1;
            unsigned vip : 1;
            unsigned id : 1;
        };
    };

    struct x86_Interrupt * interrupts[2][256];
    struct x86_Opcode * opcodes;
};

void x86_CPU_Attach(struct Machine * machine);
void x86_CPU_SetInterrupt(struct x86_CPU * cpu, unsigned char mode, unsigned char num, int (*callback)(struct x86_CPU * cpu));


#define X86_ES 0
#define X86_CS 1
#define X86_SS 2
#define X86_DS 3
#define X86_FS 4
#define X86_GS 5

inline int x86_CPU_GetAddress(struct x86_CPU * cpu, unsigned int * address, unsigned short segment, unsigned int offset);

inline int x86_CPU_LoadByte(struct x86_CPU * cpu, unsigned char * value, unsigned char sreg, unsigned int offset);
inline int x86_CPU_LoadWord(struct x86_CPU * cpu, unsigned short * value, unsigned char sreg, unsigned int offset);
inline int x86_CPU_LoadDword(struct x86_CPU * cpu, unsigned int * value, unsigned char sreg, unsigned int offset);

inline int x86_CPU_StoreByte(struct x86_CPU * cpu, unsigned char value, unsigned char sreg, unsigned int offset);
inline int x86_CPU_StoreWord(struct x86_CPU * cpu, unsigned short value, unsigned char sreg, unsigned int offset);
inline int x86_CPU_StoreDword(struct x86_CPU * cpu, unsigned int value, unsigned char sreg, unsigned int offset);

#endif // X86_CPU_H
