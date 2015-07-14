#ifndef IO_H
#define IO_H


struct IO {
    unsigned char (*inb)();
    unsigned short (*inw)();
    unsigned int (*ind)();
    void (*outb)(unsigned char v);
    void (*outw)(unsigned short v);
    void (*outd)(unsigned int v);
};

#endif // IO_H
