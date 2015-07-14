#include "memory.h"
#include <stdlib.h>
#include <string.h>


inline void Memory_Init(struct Memory * memory, unsigned int size){
    memory->size = size;
    memory->data = malloc(size);
    memset(memory->data, 0, size);
}

inline void * Memory_GetAddress(struct Memory * memory, unsigned int offset){
    return memory->data + offset;
}
