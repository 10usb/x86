#ifndef INTERRUPT_H
#define INTERRUPT_H

struct Interrupt386 {
    int (*callback)();
    unsigned char type;
    unsigned short segment;
    unsigned int offset;
};

#endif // INTERRUPT_H
