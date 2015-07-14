#ifndef MACHINE_H
#define MACHINE_H

#include <memory.h>
#include <cpu.h>
#include <io.h>
#include <video.h>
#include <stdio.h>

struct Machine {
    struct CPU * cpu;
    struct Memory memory;
    struct Video * video;
    struct IO * io[0x10000];
    FILE * floppy;
};

void Machine_LoadFloppy(struct Machine * machine, const char * file);

#endif // MACHINE_H
