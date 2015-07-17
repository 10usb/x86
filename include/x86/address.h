#ifndef ADDRESS_H
#define ADDRESS_H

#include <x86/cpu.h>

struct x86_Address {
    union {
        struct {
            unsigned char rm : 3;
            unsigned char reg : 3;
            unsigned char mode : 2;
        };
        unsigned char value;
    } modrm;
    union {
        struct {
            unsigned char base : 3;
            unsigned char index : 3;
            unsigned char mode : 2;
        };
        unsigned char value;
    } sib;
    union {
        char displ8;
        short displ16;
        int displ32;
    };
    unsigned int offset;
};

static inline int x86_Address_Load(struct x86_Address * address, int addresssize);
static inline int x86_Address_LoadB(struct x86_Address * address, struct x86_CPU * cpu);
static inline int x86_Address_LoadW(struct x86_Address * address, struct x86_CPU * cpu);
static inline int x86_Address_LoadD(struct x86_Address * address, struct x86_CPU * cpu);

#endif // ADDRESS_H
