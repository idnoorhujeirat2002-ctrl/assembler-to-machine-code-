#ifndef _DEFENETIONS_H
#define _DEFENETIONS_H

/*maximum size of code image and data image*/
#define CODE_ARR_IMG_LENGTH 4096

#define MAX_EXTERN_ENTRIES 100

/*maximum length of a single source line */
#define MAX_LINE_LENGTH 100

/*initial IC value*/
#define IC_INIT_VALUE 100

/*fixed length for the name of the file*/
#define MAX_FILENAME_LENGTH 50



/*boolean true or false definition*/
typedef enum booleans{
	false = 0,true = 1
}bool;

/*operand addressing type*/
typedef enum addressing_types{
	/*immediate addressing that turns on the 0 "lamp"*/
	IMMEDIATE_ADDR = 1,
	/*direct addressing that turns on the 1 "lamp"*/
	DIRECT_ADDR = 2,
	/*relative addressing that turns on the 2 "lamp"*/
	RELATIVE_ADDR= 4,
	/*register addressing that turns on the 3 "lamp"*/
	REGISTER_ADDR =8,
	/*failed to detected addressing*/
	NONE_ADDR =-1
}addressing_type;

/*commands opcode*/
typedef enum opcodes{

	/*first group with two operands*/
	MOV_OP=0,
	CMP_OP = 1,
	ADD_OP = 2,
	SUB_OP = 3,
	LEA_OP = 4,
	/*end of first group*/

	/*second group with one operand*/
	CLR_OP = 5,
	NOT_OP = 6,
	INC_OP = 7,
	DEC_OP = 8,
	JMP_OP = 9,
	BNE_OP = 10,
	RED_OP = 11,
	PRN_OP = 12,
	JSR_OP = 13,
	/*end of first group*/

	/*third group with no operands*/
	RTS_OP = 14,
	STOP_OP = 15,
	/*end of first group*/

	/*failed*/
	NONE_OP=-1
} opcode;

/*registers*/
typedef enum registers{
	r0,
	r1,
	r2,
	r3,
	r4,
	r5,
	r6,
	r7,
	NONE_REG= -1
}reg;

/*structure representing a single code word*/
typedef struct{
	/*three bits that represent the ARE field*/
	unsigned int ARE:3;
	/*four bits that represent the destination adress*/
	unsigned int dest_address:4;
	/*four bits that represent the source adress*/
	unsigned int src_address:4;
	/*four bits that represent the operation*/	
	unsigned int opcode:4;
}codeWord;

/*structure representing a single data word*/
typedef struct{
	/*three bits that represent the ARE field*/
	unsigned int ARE:3;
	/*data content (that can contain label line or the value of a number or the registers used ...)*/
	unsigned long data;
}dataWord;

/*structure representing a machine word in memory*/
typedef struct{
	/*represents the number of machine code words (lines) required for this instruction
	for example in the case of the instruction MAIN: add r3, LIST:
	1) the first word represents the add operation and the addressing modes for r3 and LIST
	2) the second word is needed for the first operand r3 which is a register
	3) the third word is needed for the second operand LIST which is a label or memory address
	therefore the length would be 3 in this case
	if the word is for data (not an instruction) this field is 0*/
	short length;
	/*union for either data or code content*/
	union word{
		dataWord *data;
		codeWord *code;
	}word;
}MachineWordsArray;/*represents a single code word*/

#endif
