#ifndef MEMORY_H
#define MEMORY_H


struct Memory {
    unsigned int size;
    unsigned char * data;
};

inline void Memory_Init(struct Memory * memory, unsigned int size);
inline void * Memory_GetAddress(struct Memory * memory, unsigned int offset);

#endif // MEMORY_H
