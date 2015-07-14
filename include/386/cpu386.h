#ifndef CPU386_H
#define CPU386_H

#include <cpu.h>
#include <machine.h>
#include <386/interrupt.h>

struct CPU386 {
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
    union { unsigned short ip; unsigned int eip; };
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

    struct Interrupt386 * ints[2][256];
};

void CPU386_Attach(struct Machine * machine);
void CPU386_SetInt(struct CPU386 * cpu, unsigned char mode, unsigned char num, int (*callback)(struct CPU386 * cpu));

#endif // CPU386_H
