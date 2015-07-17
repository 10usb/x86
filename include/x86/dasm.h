#ifndef X86_DASM_H
#define X86_DASM_H

#include <x86/cpu.h>
#include <x86/opcode.h>

int x86_DASM_Load(struct x86_Opcode * opcodes, const char * file);
int x86_DASM_Disassemble(const struct x86_Opcode * opcodes, int addresssize, int operandsize, const unsigned char * data, char * buffer, int size);

#endif // X86_DASM_H
