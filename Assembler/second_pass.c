#include "second_pass.h"


/*** CODE WORD HANDLING SECTION ***/

/* 
* create_code_word
* ----------------
* this function creates a codeWord struct based on the provided opcode and operands
* the function initializes the addressing types for the operands based on the opcode
* it then allocates memory for the new code word and sets the opcode and addressing types
* this function is responsible for creating the initial code word which will represent an instruction in the machine code
* 
* parameters:
*   curr_opcode: the operation code which indicates the specific operation the instruction will perform
*   op_count: the number of operands that the instruction expects 
*   operands: an array of strings representing the operands used by the instruction the operands can include register names labels or immediate values
* 
* returns:
*   codeWord*: a pointer to the newly created codeWord struct which holds the opcode and addressing information for the instruction
*/
codeWord *create_code_word(opcode curr_opcode,int op_count,char *operands[2]){
	codeWord *codeword;/*declare a pointer to hold the new code word*/
	addressing_type first_addressing=NONE_ADDR, second_addressing=NONE_ADDR;/*initialize addressing types*/

	if(operands[0]){/*if there is a first operand*/
		first_addressing=get_addressing_type(operands[0]);/*determine the addressing type for the first operand*/
	}
	if(operands[1]){/*if there is a second operand*/
		second_addressing=get_addressing_type(operands[1]);/*determine the addressing type for the second operand*/
	}

	codeword=(codeWord*)malloc(sizeof(codeWord));/*allocate memory for the new code word*/
	if(codeword==NULL){/*check if memory allocation failed*/
		printf("malloc failed");
		exit(EXIT_FAILURE);
	}

	codeword->opcode=curr_opcode;/*set the opcode in the code word*/
	codeword->ARE=4;/*default value for ARE*/
	codeword->dest_address=0;/*initialize destination address*/
	codeword->src_address=0;/*initialize source address*/

	if(curr_opcode==MOV_OP || curr_opcode==CMP_OP || curr_opcode==ADD_OP ||
			curr_opcode==SUB_OP || curr_opcode==LEA_OP){/*if the operation uses both source and destination*/
		codeword->src_address=first_addressing;/*set the source addressing type*/
		codeword->dest_address=second_addressing;/*set the destination addressing type*/
	}
	else if(curr_opcode==CLR_OP || curr_opcode==NOT_OP || curr_opcode==INC_OP ||
			curr_opcode==DEC_OP || curr_opcode==JMP_OP || curr_opcode==BNE_OP ||
			curr_opcode==JSR_OP || curr_opcode==RED_OP || curr_opcode==PRN_OP){/*if the operation uses only destination*/
		codeword->dest_address=first_addressing;/*set the destination addressing type*/
	}
	return codeword;
}

/*** DATA WORD HANDLING SECTION ***/

/* 
* handle_register_address_word
* ----------------------------
* this function creates a dataWord struct specifically for register addressing
* it handles the encoding of register numbers into the dataWord format used in the machine code
* the function checks if either the source register the destination register or both are being used
* it then shifts and combines the register numbers to create the appropriate data word
* this is important for instructions that operate on registers as it ensures the correct encoding of register values in the machine code
* 
* parameters:
*   num_of_first_reg: the number of the first register involved in the operation
*   num_of_second_reg: the number of the second register involved in the operation if any
* 
* returns:
*   dataWord*: a pointer to the created dataWord struct which encodes the register numbers into a format suitable for the machine code
*/
dataWord *handle_register_address_word(long num_of_first_reg,long num_of_second_reg){
	unsigned long ARE=4;/*set ARE value*/
	unsigned long num_of_src_reg=0, num_of_dest_reg=0;/*initialize source and destination registers*/
	dataWord *dataword=malloc(sizeof(dataWord));/*allocate memory for data word*/

	if(dataword==NULL){/*check if memory allocation failed*/
		printf("malloc failed");
		exit(EXIT_FAILURE);
	}

	dataword->ARE=ARE;/*set the ARE field*/

	if(num_of_first_reg&&(num_of_second_reg==0)){/*if only source register is used*/
		num_of_src_reg=num_of_first_reg<<3;/*shift source register value to correct position*/
		dataword->data=num_of_src_reg;/*set data in data word*/
	}
	else if((num_of_first_reg==0)&&num_of_second_reg){/*if only destination register is used*/
		num_of_dest_reg=num_of_second_reg;/*set destination register value*/
		dataword->data=num_of_dest_reg;/*set data in data word*/
	}
	else if(num_of_first_reg&&num_of_second_reg){/*if both source and destination registers are used*/
		num_of_src_reg=num_of_first_reg;/*set source register value*/
		num_of_src_reg=num_of_src_reg<<3;/*shift source register value to correct position*/
		num_of_dest_reg=num_of_second_reg;/*set destination register value*/
		dataword->data=num_of_src_reg|num_of_dest_reg;/*combine source and destination into data word*/
	}
	
	return dataword;/*return the created data word*/
}

/* 
* handle_non_register_address_word
* --------------------------------
* this function creates a dataWord struct for non-register addressing such as immediate or direct addressing
* it determines the appropriate ARE value depending on the addressing type and whether the symbol is external
* the function then stores the provided data value in the data word
* this is crucial for encoding instructions that use immediate values or references to labels as it ensures these are correctly represented in the machine code
* 
* parameters:
*   addressing: the type of addressing being used immediate direct etc
*   data: the data value that needs to be encoded in the data word this could be a number or an address
*   is_extern_symbol: a boolean flag that indicates whether the symbol being addressed is external which affects the ARE value
* 
* returns:
*   dataWord*: a pointer to the created dataWord struct which holds the encoded data and addressing information
*/
dataWord *handle_non_register_address_word(addressing_type addressing,long data,bool is_extern_symbol){
	unsigned long ARE=1;/*initialize ARE*/
	dataWord *dataword=malloc(sizeof(dataWord));/*allocate memory for data word*/

	if(dataword==NULL){/*check if memory allocation failed*/
		printf("malloc failed");
		exit(EXIT_FAILURE);
	}

	if(addressing==IMMEDIATE_ADDR){/*if addressing is immediate*/
		dataword->ARE=4;/*set ARE for immediate*/
		dataword->data=data;/*set data value*/
	}
	else if(addressing==DIRECT_ADDR){/*if addressing is direct*/
		ARE=is_extern_symbol?1:2;/*set ARE based on whether it's external*/
		dataword->ARE=ARE;/*set ARE field*/
		dataword->data=data;/*set data value*/
	}
	return dataword;/*return the created data word*/
}

/*** CODEWORD GENERATION SECTION ***/

/* 
* generate_non_register_code_word
* --------------------------------
* this function generates a machine code word for operands that are not registers
* it determines the addressing type of the operand and creates a corresponding machine word
* if the operand is an external label it ensures that the external label is properly recorded for later use
* this function is essential for handling instructions that involve memory addresses or immediate values rather than registers
* 
* parameters:
*   code_img: the array where the generated machine words are stored
*   ic: the instruction counter which tracks the current position in the code image
*   operand: the operand to be processed this could be a memory address immediate value or label
*   label_array: an array of labels used in the program which helps in resolving direct addressing
*   ext_entry_array: an array of external symbols used to track references to external labels
*   extern_printing_array: an array where information about external labels to be printed is stored
* 
*/
void generate_non_register_code_word(MachineWordsArray **code_img,long *ic,char *operand,LabelArray *label_array,ExternEntryArray *ext_entry_array,ExternEntry **extern_printing_array){
	addressing_type operand_addressing;/*declare operand addressing type*/
	MachineWordsArray *word_to_write=NULL;/*initialize word_to_write to NULL*/
	int j,i;/*loop counters*/
	int extern_count = 0;


	operand_addressing=get_addressing_type(operand);/*get the addressing type of the operand*/

	if(operand_addressing!=NONE_ADDR){/*if operand addressing is valid*/
		word_to_write=(MachineWordsArray*)malloc(sizeof(MachineWordsArray));/*allocate memory for word*/
		if(word_to_write==NULL){/*check if memory allocation failed*/
			printf("malloc failed");
			exit(EXIT_FAILURE);
		}
		if(operand_addressing==IMMEDIATE_ADDR){/*if addressing is immediate*/
			char *ptr;/*pointer for strtol*/
			short value=(short)strtol(operand+1, &ptr, 10);/*convert operand to value*/
			word_to_write->length=0;/*set word length*/
			(word_to_write->word).data=handle_non_register_address_word(IMMEDIATE_ADDR, value, false);/*set data word with the value of the operand*/
		}
		else if(operand_addressing==DIRECT_ADDR){/*if addressing is direct*/
			bool label_found = false;/*flag to check if label was found*/

			for(j=0 ; j<ext_entry_array->extern_count ; j++){/*loop through externs*/
				if(strcmp(ext_entry_array->externs[j].name, operand)==false){/*if operand matches an extern*/
					extern_count = 0;
					while(*extern_printing_array && (*extern_printing_array)[extern_count].name[0] != '\0'){/*looking for an empty space to put the newly found extern*/
						extern_count++;
					}

					*extern_printing_array = realloc(*extern_printing_array, (extern_count + 2) * sizeof(ExternEntry));/*dynamically allocates memory for the newly found extern*/
					if(*extern_printing_array == NULL){/*and ensures the reallocation passes well*/
						printf("realloc failed");
						exit(EXIT_FAILURE);
					}
					/*add the new extern and copies the name into the array ensuring not to exceed the max length*/
					strncpy((*extern_printing_array)[extern_count].name, ext_entry_array->externs[j].name, MAX_LABEL_LENGTH - 1);
					(*extern_printing_array)[extern_count].name[MAX_LABEL_LENGTH - 1] = '\0';

					(*extern_printing_array)[extern_count].line_number = *ic;/*store the line number where the extern is used*/

					(*extern_printing_array)[extern_count + 1].name[0] = '\0';/*null terminate the array*/

					word_to_write->length=0;/*set word length*/
					(word_to_write->word).data=handle_non_register_address_word(DIRECT_ADDR, 0, true);/*set data word with 0 because its an external label*/
			
					label_found = true;/*label found in extern array*/
					break;/*exit loop*/
				}
			}

			if(!label_found){/*if not found in externs, search in labels*/
				for(i=0 ; i<label_array->count ; i++){/*loop through labels*/
					if(strcmp(label_array->labels[i].label, operand)==false){/*if operand matches a label*/
						word_to_write->length=0;/*set word length*/
						(word_to_write->word).data=handle_non_register_address_word(DIRECT_ADDR, label_array->labels[i].line_number, false);/*set data word with the line of the label*/
						label_found = true;/*label found in label array*/
						break;/*exit loop*/
					}
				}
			}

			if(!label_found){/*if label is not found in either extern or label arrays*/
				fprintf(stderr, "Error: Label '%s' was not initialized\n", operand);
				exit(EXIT_FAILURE);/*terminate the program with an error*/
			}
		}

		code_img[*ic-IC_INIT_VALUE]=word_to_write;/*store the word in code image*/
		(*ic)++;/*increment instruction counter*/
	}
}

/* 
* generate_register_code_word
* ---------------------------
* this function generates a machine code word specifically for register operands
* it determines the register numbers for the operands and encodes them into a single machine word
* this function handles both source and destination registers and combines their values into the appropriate format
* it is critical for processing instructions that involve operations between registers or operations involving registers and memory
* 
* parameters:
*   code_img: the array where the generated machine words are stored
*   ic: the instruction counter which tracks the current position in the code image
*   first_operand_addr: the addressing type of the first operand indicating whether it is a register relative or other type
*   second_operand_addr: the addressing type of the second operand indicating whether it is a register relative or other type
*   operand1: the first operand which could be a register or a memory location
*   operand2: the second operand which could also be a register or a memory location
* 
*/
void generate_register_code_word(MachineWordsArray **code_img,long *ic,addressing_type first_operand_addr,addressing_type second_operand_addr,char *operand1,char *operand2){
	char *ptr;/*pointer for strtol*/
	short num_of_first_reg=0, num_of_second_reg=0;/*initialize register numbers*/
	MachineWordsArray *word_to_write;/*pointer for new word*/

	word_to_write=(MachineWordsArray*)malloc(sizeof(MachineWordsArray));/*allocate memory for word*/
	if(word_to_write==NULL){/*check if memory allocation failed*/
		printf("malloc failed");
		exit(EXIT_FAILURE);
	}

	if(first_operand_addr==REGISTER_ADDR){/*if first operand is a register*/
		num_of_first_reg=strtol(operand1+1, &ptr, 10);/*convert operand to register number*/
	}
	else if(first_operand_addr==RELATIVE_ADDR){/*if first operand is relative*/
		num_of_first_reg=strtol(operand1+2, &ptr, 10);/*convert operand to register number*/
	}
	if(second_operand_addr==REGISTER_ADDR){/*if second operand is a register*/
		num_of_second_reg=strtol(operand2+1, &ptr, 10);/*convert operand to register number*/
	}else if(second_operand_addr==RELATIVE_ADDR){/*if second operand is relative*/
		num_of_second_reg=strtol(operand2+2, &ptr, 10);/*convert operand to register number*/
	}

	word_to_write->length=0;/*set word length*/
	(word_to_write->word).data=handle_register_address_word(num_of_first_reg, num_of_second_reg);/*set data word*/
	code_img[*ic-IC_INIT_VALUE]=word_to_write;/*store the word in code image*/
	(*ic)++;/*increment instruction counter*/
}


/*** OPERATION PROCESSING SECTION ***/

/* 
* process_code
* ------------
* this function processes a line of code containing an operation and its operands
* it first determines the type of operation and then creates the appropriate machine words for the operation and its operands
* the function handles the creation of the initial code word and then generates additional words as needed based on the operands
* it is a central part of the assembler's second pass ensuring that each line of assembly code is correctly converted into machine code
* 
* parameters:
*   operation: the operation or mnemonic to be processed for example mov add etc
*   operands_line: the line containing the operands to be processed these are the values or addresses that the operation will act upon
*   ic: the instruction counter which tracks the current position in the code image
*   code_img: the array where the generated machine words are stored
*   labels: the array of labels used in the program which helps in resolving direct addressing
*   ext_entry_array: an array of external symbols used to track references to external labels
*   extern_printing_array: an array where information about external labels to be printed is stored
* 
* returns:
*   bool: true if the processing was successful and false otherwise this indicates whether the line of code was correctly converted into machine code
*/
bool process_code(const char *operation,const char *operands_line,long *ic,MachineWordsArray **code_img,LabelArray *labels,ExternEntryArray *ext_entry_array,ExternEntry **extern_printing_array){
	char *token;/*pointer for strtok*/
	int i = 0;/*operand counter*/
	char *operands[2] = {NULL, NULL};/*array for operands*/
	opcode curr_opcode;/*current opcode*/
	codeWord *codeword;/*pointer for code word*/
	long ic_before;/*instruction counter before changes*/
	int operand_count;/*number of operands*/
	MachineWordsArray *word_to_write;/*pointer for new word*/
	addressing_type first_operand_addressing = NONE_ADDR, second_operand_addressing = NONE_ADDR;/*initialize addressing types*/

	if(operands_line != NULL && *operands_line != '\0'){/*if there are operands*/
		token = strtok((char*)operands_line, ", \t");/*split operands*/
		while(token != NULL && i < 2){/*loop through operands*/
			operands[i] = malloc(MAX_OPERAND_LENGTH * sizeof(char));/*allocate memory for operand*/
			if(operands[i] == NULL){/*check if memory allocation failed*/
				printf("Error: malloc failed for operand %d\n", i);
				return false;
			}
			strncpy(operands[i], token, MAX_OPERAND_LENGTH);/*copy operand*/
			operands[i][MAX_OPERAND_LENGTH - 1] = '\0';/*null terminate operand*/
			i++;/*increment operand counter*/
			token = strtok(NULL, ", \t");/*get next operand*/
		}

		if(i == 0){/*if no operands were extracted*/
			printf("Error: Operands not extracted correctly.\n");
			return false;
		}
	}

	curr_opcode = get_opcode(operation);/*get opcode from operation*/
	if(curr_opcode == NONE_OP){/*if operation is unrecognized*/
		printf("Error: Unrecognized operation: %s\n", operation);
		return false;
	}

	operand_count = get_operand_count(operation);/*get number of operands*/

	codeword = create_code_word(curr_opcode, operand_count, operands);/*create code word*/
	if(codeword == NULL){/*if code word creation failed*/
		printf("Error: Failed to create code word for operation: %s\n", operation);
		return false;
	}

	ic_before = *ic;/*save instruction counter state*/

	word_to_write = (MachineWordsArray*)malloc(sizeof(MachineWordsArray));/*allocate memory for new word*/
	if(word_to_write == NULL){/*check if memory allocation failed*/
		printf("Error: malloc failed for word_to_write\n");
		free(codeword);/*free code word memory*/
		return false;
	}

	word_to_write->word.code = codeword;/*set code word*/
	code_img[(*ic) - IC_INIT_VALUE] = word_to_write;/*store the word in code image*/
	(*ic)++;/*increment instruction counter*/

	if(operands[0]){/*if there is a first operand*/
		first_operand_addressing = get_addressing_type(operands[0]);/*get addressing type*/
		if(first_operand_addressing == -1){/*if invalid addressing type*/
			printf("Error: Invalid addressing type for operand 1: %s\n", operands[0]);
			return false;
		}
	}
	if(operands[1]){/*if there is a second operand*/
		second_operand_addressing = get_addressing_type(operands[1]);/*get addressing type*/
		if(second_operand_addressing == -1){/*if invalid addressing type*/
			printf("Error: Invalid addressing type for operand 2: %s\n", operands[1]);
			return false;
		}
	}

	if(operand_count--){/*if there are operands to process*/
		if((first_operand_addressing == REGISTER_ADDR || first_operand_addressing == RELATIVE_ADDR) &&
			(second_operand_addressing == REGISTER_ADDR || second_operand_addressing == RELATIVE_ADDR)){/*if both operands are registers or relative*/
			generate_register_code_word(code_img, ic, first_operand_addressing, second_operand_addressing, operands[0], operands[1]);/*generate register code word*/
			operand_count--;/*decrement operand count*/
		}
		else if(first_operand_addressing == REGISTER_ADDR || first_operand_addressing == RELATIVE_ADDR){/*if first operand is register or relative*/
			if(!operand_count){/*if only one operand*/
				generate_register_code_word(code_img, ic, NONE_ADDR, first_operand_addressing, NULL, operands[0]);/*generate code word for destination register if the operation takes one operand*/
			}
			else if(operand_count--){/*if more than one operand*/
				generate_register_code_word(code_img, ic, first_operand_addressing, second_operand_addressing, operands[0], NULL);/*generate register code word as the source operand*/
				generate_non_register_code_word(code_img, ic, operands[1], labels, ext_entry_array, extern_printing_array);/*generate code word for second operand*/
			}
		}
		else if(second_operand_addressing == REGISTER_ADDR || second_operand_addressing == RELATIVE_ADDR){/*if second operand is register or relative*/
			if(operand_count--){/*if more than one operand*/
				generate_non_register_code_word(code_img, ic, operands[0], labels, ext_entry_array, extern_printing_array);/*generate code word for first operand*/
			}
			generate_register_code_word(code_img, ic, first_operand_addressing, second_operand_addressing, NULL, operands[1]);/*generate register code word as the destination operand*/
		}
		else{ /*if no register operands*/
			generate_non_register_code_word(code_img, ic, operands[0], labels, ext_entry_array, extern_printing_array);/*generate code word for first operand*/
			if(operand_count > 0){/*if more than one operand*/
				generate_non_register_code_word(code_img, ic, operands[1], labels, ext_entry_array, extern_printing_array);/*generate code word for second operand*/
			}
		}
	} /*NOTE: the calls for generating the words are made in a specific sequence to maintain the correct order when printing the operands*/

	/* Set the length of the machine word. This represents the total number of words required for the instruction. */
	code_img[ic_before - IC_INIT_VALUE]->length = (*ic) - ic_before;/*set length of machine word*/

	free(operands[0]);
	free(operands[1]);
	return true;
}



/*** DIRECTIVE HANDLING SECTION ***/

/* 
* handle_string_directive_SP
* --------------------------
* this function handles the .string directive during the second pass of the assembler
* it extracts the string from the directive and stores each character as an entry in the data image array followed by a null terminator
* this function ensures that strings are correctly represented in the assembled machine code allowing them to be used in the program's data segment
* 
* pparameters:
*   line: the line containing the .string directive including the string to be processed
*   data_img: the array where the string's characters will be stored in the program's data segment
*   dc: the data counter which tracks the current position in the data image array
* 
* returns:
*   bool: true if the string was successfully processed and stored and false if there was an error
*/
bool handle_string_directive_SP(const char *line,long *data_img,long *dc){
	char *start;/*start of string*/
	char *end;/*end of string*/

	while(*line==' '||*line=='\t'){/*skip leading whitespace*/
		line++;
	}

	if(*line!='\"'){/*if the string doesn't start with a quote*/
		printf("Error: .string directive does not start with a quote.\n");
		return false;
	}

	start=(char*)(line+1);/*set start to first character after quote*/

	end=strchr(start, '\"');/*find closing quote*/
	if(end==NULL){/*if there is no closing quote*/
		printf("Error: .string directive does not have a closing quote.\n");
		return false;
	}

	while(start<end){/*loop through string characters*/
		data_img[*dc]=(long)(*start);/*store character in data image*/
		(*dc)++;/*increment data counter*/
		start++;/*move to next character*/
	}

	data_img[*dc]=(long)'\0';/*add null terminator to string*/
	(*dc)++;/*increment data counter*/
	return true;/*return success*/
}

/* 
* handle_data_directive_SP
* ------------------------
* this function handles the .data directive during the second pass of the assembler
* it processes a line containing numeric values and stores each value as an entry in the data image array
* this function is crucial for converting numeric data in the source code into a format that can be used in the program's data segment
* 
* parameters:
*   line: the line containing the .data directive including the values to be processed
*   data_img: the array where the numeric values will be stored in the program's data segment
*   dc: the data counter which tracks the current position in the data image array
* 
* returns:
*   bool: true if the data was successfully processed and stored and false if there was an error
*/
bool handle_data_directive_SP(const char *line,long *data_img,long *dc){
	char *token;/*pointer for strtok*/
	char *endptr;/*pointer for strtol*/
	char *original_line=(char*)malloc(strlen(line)+1);/*allocate memory for line copy*/
	char *line_copy;/*copy of line*/
	long value;/*data value*/

	if(original_line==NULL){/*if memory allocation failed*/
		fprintf(stderr, "Error: Memory allocation failed for line copy\n");
		return false;
	}

	strcpy(original_line, line);/*copy line to allocated memory*/
	line_copy=original_line;/*set line copy pointer*/

	token=strtok(line_copy, " \t,");/*split line into tokens*/
	while(token){/*loop through tokens*/
		value=strtol(token, &endptr, 10);/*convert token to long*/

		if(*endptr!='\0'){/*if the token is not a valid number*/
			fprintf(stderr, "Error: Invalid number '%s' in .data directive\n", token);/*print error message*/
			free(original_line);/*free memory*/
			return false;/*return failure*/
		}
		data_img[*dc]=value;/*store value in data image*/
		(*dc)++;/*increment data counter*/
		token=strtok(NULL, " \t,");/*get next token*/
	}

	free(original_line);/*free memory*/
	return true;/*return success*/
}

/*** SECOND PASS MAIN FUNCTION ***/

/* 
* second_pass
* -----------
* this function performs the second pass of the assembler where it processes the assembly code to generate the final machine code
* during this pass it handles the conversion of instructions and directives into their corresponding machine code representations
* it reads the source file line by line identifying operations and directives and invoking the appropriate functions to process them
* it also manages the allocation of memory for the machine code and data segments and tracks external symbols for later use
* this function is the final step in assembling the source code ensuring that all instructions and data are correctly encoded and ready for output
* 
* parameters:
*   filename: the name of the file containing the assembly source code to be processed
*   label_array: an array of labels used in the program which helps in resolving direct addressing
*   ext_entry_array: an array of external symbols used to track references to external labels
*   code_img: the array where the generated machine words are stored representing the programs instructions
*   data_img: the array where the programs data is stored representing the programs data segment
*   IC: a pointer to the instruction counter which tracks the current position in the code image
*   DC: a pointer to the data counter which tracks the current position in the data image
*   extern_printing_array: an array where information about external labels to be printed is stored allowing the assembler to track and output extern usage
* 
* returns:
*   bool: true if the second pass was successful and false otherwise this indicates whether the assembly process completed without errors
*/
bool second_pass(const char *filename,LabelArray *label_array,ExternEntryArray *ext_entry_array,MachineWordsArray **code_img,long *data_img,long *IC,long *DC,ExternEntry **extern_printing_array){
	FILE *file;/*pointer to file*/
	char line[MAX_LINE_LENGTH+1];/*buffer for reading lines*/
	char *operation=NULL;/*pointer for operation*/
	char *remaining_line=NULL;/*pointer for remaining line*/
	bool successful=true;/*success flag*/
	*extern_printing_array = NULL;/*initialize the extern printing array*/

	file=fopen(filename, "r");/*open the input file*/
	if(!file){/*if file open failed*/
		perror("Error opening file");/*print error message*/
		return false;/*return failure*/
	}

	while(fgets(line, sizeof(line), file)){/*read a line from the input file*/
		line[strcspn(line, "\r\n")]=0;/*trim newline characters*/

		operation=strtok(line, " \t");/*split line to get operation or label*/
		if(operation&&strchr(operation, ':')!=NULL){/*if a label is present*/

			operation=strtok(NULL, " \t");/*get the operation following the label*/

			if(operation==NULL){/*if no operation after label*/
				continue;/*skip this line*/
			}

			remaining_line=strtok(NULL, "");/*get the rest of the line*/
		}
		else{
			remaining_line=strtok(NULL, "");/*get the rest of the line*/
		}

		if(operation==NULL || *operation=='\0'){/*if no operation found*/
			continue;/*skip this line*/
		}

		if(remaining_line==NULL){/*if there is no remaining line*/
			remaining_line="";/*set remaining line to empty string*/
		}

		if(strcmp(operation, ".extern")==false || strcmp(operation, ".entry")==false){/*if the operation is .extern or .entry*/
			continue;/*skip these lines in the second pass (handled in the first pass)*/
		}
		else if(strcmp(operation, ".data")==false){/*if the operation is .data*/
			successful=handle_data_directive_SP(remaining_line, data_img, DC);/*handle the .data directive*/
			if(!successful){/*if handling failed*/
				fprintf(stderr, "Error processing .data directive on line %ld\n", *IC);
				fclose(file);/*close the file*/
				return false;/*return failure*/
			}
		}
		else if(strcmp(operation, ".string")==false){/*if the operation is .string*/
			successful=handle_string_directive_SP(remaining_line, data_img, DC);/*handle the .string directive*/
			if(!successful){/*if handling failed*/
				fprintf(stderr, "Error processing .string directive on line %ld\n", *IC);
				fclose(file);/*close the file*/
				return false;/*return failure*/
			}
		}
		else if(is_operation(operation)){/*if the operation is a valid operation*/
			successful=process_code(operation, remaining_line, IC, code_img, label_array, ext_entry_array, extern_printing_array);/*process the operation*/
			if(!successful){/*if processing failed*/
				fclose(file);/*close the file*/
				return false;/*return failure*/
			}
		}
		else{/*if the operation is unrecognized*/
			fprintf(stderr, "Unrecognized operation or directive: %s\n", operation);/*print error message*/
			fclose(file);/*close the file*/
			return false;/*return failure*/
		}
	}

	fclose(file);/*close the file*/
	return true;/*return success*/
}

