#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#define MAX_OPERATION_LENGTH 10
#define MAX_OPERAND_LENGTH 31

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include "first_pass.h"
#include "utils.h"
#include "definitions.h"

/*** CODE WORD HANDLING SECTION ***/

codeWord *create_code_word(opcode curr_opcode,int op_count,char *operands[2]); /* function to create a code word */

/*** DATA WORD HANDLING SECTION ***/

dataWord *handle_register_address_word(long num_of_first_reg,long num_of_second_reg);/*function to handle data word creation for register addressing*/
dataWord *handle_non_register_address_word(addressing_type addressing,long data,bool is_extern_symbol);/*function to handle data word creation for non-register addressing*/

/*** CODEWORD GENERATION SECTION ***/

void generate_non_register_code_word(MachineWordsArray **code_img,long *ic,char *operand,LabelArray *labels,ExternEntryArray *ext_entry_array,ExternEntry **extern_printing_array);/*function to generate code word for non-register operands*/
void generate_register_code_word(MachineWordsArray **code_img,long *ic,addressing_type first_operand_addr,addressing_type second_operand_addr,char *operand1,char *operand2);/* function to generate code word for register operands*/

/*** OPERATION PROCESSING SECTION ***/

bool process_code(const char *operation,const char *operands_line,long *ic,MachineWordsArray **code_img,LabelArray *labels,ExternEntryArray *ext_entry_array,ExternEntry **extern_printing_array);/*function to process an operation and generate corresponding machine code*/

/*** DIRECTIVE HANDLING SECTION ***/

bool handle_data_directive_SP(const char *line,long *data_img,long *dc);/*function to handle .data directive in the second pass*/
bool handle_string_directive_SP(const char *line,long *data_img,long *dc);/*function to handle .string directive in the second pass*/

/*** SECOND PASS MAIN FUNCTION ***/

bool second_pass(const char *filename,LabelArray *label_array,ExternEntryArray *ext_entry_array,MachineWordsArray **code_img,long *data_img,long *IC,long *DC,ExternEntry **extern_printing_array);/*main function for the second pass*/

#endif /* SECOND_PASS_H */

