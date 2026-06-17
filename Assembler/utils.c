#include "utils.h"

/* 
* is_alpha
* --------
* this function checks if a given character is an alphabetic letter
* it returns true if the character is within the range of uppercase or lowercase letters (A-Z or a-z)
* this function is useful for validating whether a character is a valid starting character for a label or identifier
* 
* parameters:
*   c: the character to check
* 
* returns:
*   bool: true if the character is alphabetic, false otherwise
*/
bool is_alpha(char c) {
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) ? true : false;
}

/* 
* is_alnum
* --------
* this function checks if a given character is alphanumeric, meaning it is either a letter or a digit
* it combines the check for alphabetic characters (using is_alpha) and digits (0-9)
* this function is useful for validating that a character is part of a valid label or identifier in assembly language
* 
* parameters:
*   c: the character to check
* 
* returns:
*   bool: true if the character is alphanumeric, false otherwise
*/
bool is_alnum(char c) {
	return (is_alpha(c) || (c >= '0' && c <= '9')) ? true : false;
}

/* 
* is_digit
* --------
* this function checks if a given character is a digit
* it returns true if the character is within the range of numeric digits (0-9)
* this function is useful for validating numerical values in immediate addressing or register numbers
* 
* parameters:
*   c: the character to check
* 
* returns:
*   bool: true if the character is a digit, false otherwise
*/
bool is_digit(char c) {
	return (c >= '0' && c <= '9') ? true : false;
}

/* 
* is_valid_operand
* ----------------
* this function checks if a given operand is valid according to assembly language rules
* it validates the operand based on its type (immediate, register, relative address, or label)
* this function is essential for ensuring that operands used in instructions are properly formed and adhere to expected formats
* 
* parameters:
*   operand: the operand string to check
* 
* returns:
*   bool: true if the operand is valid, false otherwise
*/
bool is_valid_operand(const char *operand){
	int i;
	int reg_num;
	if(operand[0] == '#'){
		for(i=1; operand[i]!='\0'; i++){
			if(!is_digit(operand[i]) && operand[i] != '-' && operand[i] != '+'){
				return false;/*not a valid immediate value*/
			}
		}
	}
	else if(operand[0] == 'r'){
		if(is_digit(operand[1]) && operand[2] == '\0'){
			reg_num = operand[1] - '0';
			if(reg_num < 0 || reg_num > 7){
				return false;/*register number is invalid*/
			}
		}
		else{
			return false;/*not a valid register*/
		}
	}
	else if(operand[0] == '*'){
		if(operand[1] == 'r' && is_digit(operand[2]) && operand[3] == '\0'){
			reg_num = operand[2] - '0';
			if(reg_num < 0 || reg_num > 7){
				return false;/*relative address is invalid*/
			}
		}
		else{
			return false;/*invalid relative address*/
		}
	} else if (is_alpha(operand[0])) {
		for(i=1 ; operand[i]!='\0' ; i++){
			if (!is_alnum(operand[i])) {
				return false;/*invalid label*/
			}
		}
	}
	else{
		return false;/*invalid address*/
	}
	return true;/*operand is valid*/
}

/* 
* get_addressing_type
* -------------------
* this function determines the addressing type of a given operand
* it returns the appropriate addressing_type enumeration based on the format of the operand
* this function is key to identifying how each operand should be processed during the assembly of instructions
* 
* parameters:
*   operand: the operand string to analyze
* 
* returns:
*   addressing_type: the type of addressing used by the operand
*/
addressing_type get_addressing_type(char *operand){
	int reg_num;
	if(operand[0] == '#'){
		return IMMEDIATE_ADDR;
	}
	else if (is_alpha(operand[0])){
		if(operand[0] == 'r' && is_digit(operand[1]) && operand[2] == '\0'){
			reg_num = operand[1] - '0';
			if(reg_num >= 0 && reg_num <= 7) {
				return REGISTER_ADDR;
			}
		}
		else{
			return DIRECT_ADDR;
		}
	}
	else if (operand[0] == '*'){
		return RELATIVE_ADDR;
	}
	return NONE_ADDR;
}

/* 
* get_opcode
* ----------
* this function returns the opcode associated with a given operation name
* it uses a series of string comparisons to map the operation name to the corresponding opcode enumeration
* this function is essential for translating assembly instructions into their opcode representations
* 
* parameters:
*   operation: the name of the operation
* 
* returns:
*   opcode: the corresponding opcode or NONE_OP if the operation is not recognized
*/
opcode get_opcode(const char *operation){
    if (strcmp(operation, "mov") == 0) return MOV_OP;
    if (strcmp(operation, "cmp") == 0) return CMP_OP;
    if (strcmp(operation, "add") == 0) return ADD_OP;
    if (strcmp(operation, "sub") == 0) return SUB_OP;
    if (strcmp(operation, "lea") == 0) return LEA_OP;
    if (strcmp(operation, "clr") == 0) return CLR_OP;
    if (strcmp(operation, "not") == 0) return NOT_OP;
    if (strcmp(operation, "inc") == 0) return INC_OP;
    if (strcmp(operation, "dec") == 0) return DEC_OP;
    if (strcmp(operation, "jmp") == 0) return JMP_OP;
    if (strcmp(operation, "bne") == 0) return BNE_OP;
    if (strcmp(operation, "red") == 0) return RED_OP;
    if (strcmp(operation, "prn") == 0) return PRN_OP;
    if (strcmp(operation, "jsr") == 0) return JSR_OP;
    if (strcmp(operation, "rts") == 0) return RTS_OP;
    if (strcmp(operation, "stop") == 0) return STOP_OP;
    return NONE_OP;
}
