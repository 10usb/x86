#ifndef CPU_H
#define CPU_H


struct CPU {
    void (*run)(struct CPU * cpu);
    void (*setint)(struct CPU * cpu, unsigned char mode, unsigned char num, int (*callback)());
};

#endif // CPU_H
