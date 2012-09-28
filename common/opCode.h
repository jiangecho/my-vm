#ifndef OPCODE_H_
#define OPCODE_H_
// opcodes of azi assembler language

#define NOP		0X01
#define PUSH	0x02
#define POP		0x03
#define ICONST	0x04
#define ILOAD	0x06

#define ADD		0x07
#define RET_V	0x08
#define CALL	0x09
#define RET_I	0x0a

// the user will be able to define his own varilable
#define INT	0x0b
#define	ISTORE	0x0c


#endif
