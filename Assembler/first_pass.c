#include "first_pass.h"

/*** ARRAYS SECTION ***/

/*
* operations
* ----------
* this array contains the names of all supported assembly operations
* it is used to validate and identify operations in the source code during the first pass
* having these operations in an array allows for easy lookup and comparison
*/
const char *operations[] = {
	"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

/*
* operations_info
* ---------------
* this array contains detailed information about each operation including the valid addressing types for source and destination operands
* it is used to validate the types of operands passed to each operation ensuring they adhere to the assembly language rules
* each entry corresponds to an operation and specifies which addressing modes are allowed for the source and destination
*/
operation_info operations_info[] = {
	{"mov", {IMMEDIATE_ADDR, DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1}, {DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1}},
	{"cmp", {IMMEDIATE_ADDR, DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1}, {IMMEDIATE_ADDR, DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1}},
	{"add", {IMMEDIATE_ADDR, DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1}, {DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1}},
	{"sub", {IMMEDIATE_ADDR, DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1}, {DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1}},
	{"lea", {DIRECT_ADDR, -1, -1, -1, -1}, {DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1, -1}},
	{"clr", {-1, -1, -1, -1, -1}, {DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1, -1}},
	{"not", {-1, -1, -1, -1, -1}, {DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1, -1}},
	{"inc", {-1, -1, -1, -1, -1}, {DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1, -1}},
	{"dec", {-1, -1, -1, -1, -1}, {DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1, -1}},
	{"jmp", {-1, -1, -1, -1, -1}, {DIRECT_ADDR, RELATIVE_ADDR, -1, -1, -1}},
	{"bne", {-1, -1, -1, -1, -1}, {DIRECT_ADDR, RELATIVE_ADDR, -1, -1, -1}},
	{"red", {-1, -1, -1, -1, -1}, {DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1, -1}},
	{"prn", {-1, -1, -1, -1, -1}, {IMMEDIATE_ADDR, DIRECT_ADDR, RELATIVE_ADDR, REGISTER_ADDR, -1}},
	{"jsr", {-1, -1, -1, -1, -1}, {DIRECT_ADDR, RELATIVE_ADDR, -1, -1, -1}},
	{"rts", {-1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1}},
	{"stop", {-1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1}}
};

/*** OPERATION HANDLING SECTION ***/

/* 
* is_operation
* ------------
* this function checks whether a given token is a valid operation
* it compares the token against the predefined list of supported operations
* this function helps in identifying if a part of the source code is an operation or not
* 
* parameters:
*   token: the token to check
* 
* returns:
*   bool: true if the token is a valid operation, false otherwise
*/
bool is_operation(const char *token){
	int i;
	for(i=0 ; i<NUM_OPERATIONS ; i++){
		if(strcmp(token,operations[i]) == 0){
			return true;/*token matches one of the operations*/
		}
	}
	return false;/*no operation matches*/
}

/* 
* get_operand_count
* -----------------
* this function returns the number of operands required for a given operation
* it determines this by comparing the operation name against known operations with their expected operand counts
* this function helps in validating that the correct number of operands are provided for each operation
* 
* parameters:
*   operation: the name of the operation
* 
* returns:
*   int: the number of operands required or -1 if the operation is invalid
*/
int get_operand_count(const char *operation) {
	if(strcmp(operation,"mov") == 0 || strcmp(operation,"cmp") == 0 ||
		strcmp(operation,"add") == 0 || strcmp(operation,"sub") == 0 || 
		strcmp(operation,"lea") == 0){
		return 2;/*return 2 operands for these operations*/
	}
	else if(strcmp(operation,"clr") == 0 || strcmp(operation,"not") == 0 ||
			   strcmp(operation,"inc") == 0 || strcmp(operation,"dec") == 0 ||
			   strcmp(operation,"jmp") == 0 || strcmp(operation,"bne") == 0 ||
			   strcmp(operation,"red") == 0 || strcmp(operation,"prn") == 0 ||
			   strcmp(operation,"jsr") == 0){
		return 1;/*return 1 operand for these operations*/
	}
	else if(strcmp(operation,"rts") == 0 || strcmp(operation,"stop") == 0){
		return 0;/*return 0 operands for these operations*/
	}
	return -1;/*invalid operations*/
}

/* 
* is_valid_operand_type
* ---------------------
* this function checks if a given operand type is valid for an operation
* it compares the operand type against the list of valid types for that operation
* this function is crucial for ensuring that the operands used with an operation are of the correct type
* 
* parameters:
*   type: the addressing type of the operand
*   valid_types: an array of valid addressing types for the operation
* 
* returns:
*   int: true if the type is valid false otherwise
*/
int is_valid_operand_type(int type,int *valid_types){
	int i;
	for(i=0 ; valid_types[i]!=-1 ; i++){
		if(type == valid_types[i]){
			return true;/*type matches one of the valid types*/
		}
	}
	return false;/*no valid type matches*/
}

/* 
* get_operation_info
* ------------------
* this function retrieves the operation_info structure for a given operation
* it searches through the operations_info array to find the matching operation
* this function provides the necessary details for validating operands and calculating instruction lines
* 
* parameters:
*   operation: the name of the operation
* 
* returns:
*   operation_info*: a pointer to the operation_info structure or NULL if the operation is not found
*/
operation_info *get_operation_info(const char *operation){
	int i;
	for(i=0 ; i<NUM_OPERATIONS ; i++){
		if(strcmp(operation,operations_info[i].operation) == 0){
			return &operations_info[i];/*operation matches*/
		}
	}
	return NULL;/*no operation matches*/
}

/* 
* calculate_instruction_lines
* ---------------------------
* this function calculates the number of lines required to represent an operation and its operands in the machine code
* it determines the addressing types of the operands and adjusts the line count accordingly
* this function ensures that the correct number of lines are allocated in the code image for each instruction
* 
* parameters:
*   operation: the name of the operation
*   operands: the string containing the operands for the operation
* 
* returns:
*   int: the total number of lines needed for the operation and its operands
*/
int calculate_instruction_lines(char *operation,char *operands){
	char *token;
	addressing_type src_type = NONE_ADDR;
	addressing_type dest_type = NONE_ADDR;
	int num_of_lines = 1;/*initialize with 1 for the instruction itself*/
	int operand_count = 0;/*initialize operand count*/
	int expected_operand_count;
	operation_info *op_info;

	op_info = get_operation_info(operation);/*get the operation info*/
	if(!op_info){
		fprintf(stderr,"Error: Unknown operation '%s'\n",operation);
		exit(EXIT_FAILURE);
	}

	token = strtok(operands," ,\t");/*get the first operand*/
	while(token){
		operand_count++;
		if(operand_count == 1){
			src_type = get_addressing_type(token);/*determine the addressing type of the first operand*/
		}
		else if(operand_count == 2){
			dest_type = get_addressing_type(token);/*determine the addressing type of the second operand*/
		}
		else{
			fprintf(stderr,"Error: Too many operands for operation '%s'\n",operation);
			exit(EXIT_FAILURE);
		}
		if(!is_valid_operand(token)){
			fprintf(stderr,"Error: Invalid operand '%s' for operation '%s'\n",token,operation);
			exit(EXIT_FAILURE);
		}
		token = strtok(NULL," ,\t");/*get the next operand*/
	}

	expected_operand_count = get_operand_count(operation);/*get the expected operand count for the operation*/
	if(expected_operand_count != operand_count){
		if(operand_count < expected_operand_count){
			fprintf(stderr,"Error: Missing operands for operation '%s'\n",operation);
		}
		else{
			fprintf(stderr,"Error: Too many operands for operation '%s'\n",operation);
		}
		exit(EXIT_FAILURE);
	}

	/*validate operand types*/
	if(operand_count == 1){
		if(!is_valid_operand_type(src_type,op_info->valid_dest_types)){
			fprintf(stderr,"Error: Invalid operand type for operation '%s'\n",operation);
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(operand_count >= 1 && !is_valid_operand_type(src_type,op_info->valid_src_types)){
			fprintf(stderr,"Error: Invalid source operand type for operation '%s'\n",operation);
			exit(EXIT_FAILURE);
		}
		if(operand_count == 2 && !is_valid_operand_type(dest_type,op_info->valid_dest_types)){
			fprintf(stderr,"Error: Invalid destination operand type for operation '%s'\n",operation);
			exit(EXIT_FAILURE);
		}
	}

	/*adjust the number of lines based on addressing types*/
	if(src_type != NONE_ADDR)num_of_lines++;/*add line for source operand if exists*/
	if(dest_type != NONE_ADDR)num_of_lines++;/*add line for destination operand if exists*/
	if((src_type == REGISTER_ADDR || src_type == RELATIVE_ADDR) && (dest_type == REGISTER_ADDR || dest_type == RELATIVE_ADDR)){
		num_of_lines--;/*if both are registers or relative addresses they share a line*/
	}

	return num_of_lines;
}

/*** LABEL HANDLING SECTION ***/

/* 
* initialize_label_array
* ----------------------
* this function initializes a LabelArray structure to store labels encountered during the first pass
* it allocates memory for the initial array of labels and sets the initial count and capacity
* this function is necessary to ensure labels are correctly stored and managed during the assembly process
* 
* parameters:
*   array: a pointer to the LabelArray structure to be initialized
* 
*/
void initialize_label_array(LabelArray *array){
	array->count = 0;/*initialize count*/
	array->capacity = 10;/*initial capacity*/
	array->labels = (Label*)malloc(array->capacity * sizeof(Label));/*allocate memory for labels*/
	if(array->labels == NULL){
		fprintf(stderr,"Error: Memory allocation failed for label array\n");
		exit(EXIT_FAILURE);
	}
}

/* 
* is_unique_label
* ---------------
* this function checks if a given label is unique within the LabelArray
* it ensures that no duplicate labels are present in the array which could lead to errors during assembly
* this function is important for maintaining the integrity of label usage in the source code
* 
* parameters:
*   array: a pointer to the LabelArray structure containing all labels
*   label: the label to check for uniqueness
* 
* returns:
*   bool: true if the label is unique, false otherwise
*/
bool is_unique_label(LabelArray *array,const char *label){
	int i;
	for(i=0 ; i<array->count ; i++){
		if(strcmp(array->labels[i].label,label) == 0){
			return false;/*label already exists*/
		}
	}
	return true;/*label is unique*/
}

/* 
* add_label
* ---------
* this function adds a new label to the LabelArray after ensuring it is unique
* if the array is full it reallocates memory to accommodate more labels
* this function is essential for recording label definitions along with their line numbers
* 
* parameters:
*   array: a pointer to the LabelArray structure where the label will be added
*   label: the name of the label to add
*   line_number: the line number where the label was found in the source code
* 
*/
void add_label(LabelArray *array,const char *label,int line_number){
	if(!is_unique_label(array,label)){/*check if the label is unique*/
		fprintf(stderr,"Error: Duplicate label '%s' found on line %d\n",label,line_number);
		exit(EXIT_FAILURE);
	}
	if(array->count >= array->capacity){/*check if array is full*/
		array->capacity *= 2;/*double the capacity*/
		array->labels = (Label*)realloc(array->labels,array->capacity * sizeof(Label));/*reallocate memory*/
		if(array->labels == NULL){
			fprintf(stderr,"Error: Memory reallocation failed for label array\n");
			exit(EXIT_FAILURE);
		}
	}
	strncpy(array->labels[array->count].label,label,MAX_LABEL_LENGTH);/*copy label to the label array*/
	array->labels[array->count].label[MAX_LABEL_LENGTH] = '\0';/*ensure null termination*/
	array->labels[array->count].line_number = line_number;/*store the line number where the label was found*/
	array->count++;/*increment the label count*/
}

/* 
* is_valid_label
* --------------
* this function checks if a given label is valid according to assembly language rules
* it ensures that the label starts with a letter and is alphanumeric
* this function is critical for validating label names to prevent errors in the assembly process
* 
* parameters:
*   label: the label to check for validity
* 
* returns:
*   bool: true if the label is valid, false otherwise
*/
bool is_valid_label(const char *label){
	int i;
	if(!is_alpha(label[0]))return false;/*label must start with a letter*/
	for(i=1 ; label[i]!='\0' ; i++){
		if(!is_alnum(label[i]))return false;/*label must be alphanumeric*/
	}
	return true;/*label is valid*/
}

/*** EXTERN AND ENTRY HANDLING SECTION ***/

/* 
* initialize_extern_entry_array
* -----------------------------
* this function initializes an ExternEntryArray structure to store extern and entry directives encountered during the first pass
* it sets the initial counts for externs and entries
* this function is necessary for managing extern and entry references in the assembly process
* 
* parameters:
*   array: a pointer to the ExternEntryArray structure to be initialized
* 
*/
void initialize_extern_entry_array(ExternEntryArray *array){
	array->extern_count = 0;/*initialize extern count*/
	array->entry_count = 0;/*initialize entry count*/
}

/* 
* add_extern
* ----------
* this function adds a new extern to the ExternEntryArray
* it stores the externs name and the line number where it was found
* this function is crucial for tracking extern references in the source code
* 
* parameters:
*   array: a pointer to the ExternEntryArray structure where the extern will be added
*   name: the name of the extern to add
*   line_number: the line number where the extern was found
* 
*/
void add_extern(ExternEntryArray *array,const char *name,int line_number){
	if(array->extern_count >= MAX_EXTERN_ENTRIES){/*check if array is full*/
		fprintf(stderr,"Error: Extern array is full\n");
		exit(EXIT_FAILURE);
	}
	strncpy(array->externs[array->extern_count].name,name,MAX_LABEL_LENGTH);/*copy extern name*/
	array->externs[array->extern_count].line_number = line_number;/*store the line number where the extern was found*/
	array->extern_count++;/*increment the extern count*/
}

/* 
* add_entry
* ---------
* this function adds a new entry to the ExternEntryArray
* it stores the entrys name and the line number where it was found
* this function is essential for tracking entry references in the source code
* 
* parameters:
*   array: a pointer to the ExternEntryArray structure where the entry will be added
*   name: the name of the entry to add
*   line_number: the line number where the entry was found
* 
*/
void add_entry(ExternEntryArray *array,const char *name,int line_number){
	if(array->entry_count >= MAX_EXTERN_ENTRIES){/*check if array is full*/
		fprintf(stderr,"Error: Entry array is full\n");
		exit(EXIT_FAILURE);
	}
	strncpy(array->entries[array->entry_count].name,name,MAX_LABEL_LENGTH);/*copy entry name*/
	array->entries[array->entry_count].line_number = line_number;/*store the line number where the entry was found*/
	array->entry_count++;/*increment the entry count*/
}

/* 
* handle_extern_entry_directive
* -----------------------------
* this function processes the .extern and .entry directives in the source code
* it adds each extern or entry to the appropriate array within the ExternEntryArray structure
* this function is key to ensuring that externs and entries are correctly handled in the assembly process
* 
* parameters:
*   directive: the directive (.extern or .entry) being processed
*   line: the line of code containing the directive
*   ext_entry_array: a pointer to the ExternEntryArray structure where externs and entries will be stored
*   line_number: the line number where the directive was found
* 
*/
void handle_extern_entry_directive(char *directive,char *line,ExternEntryArray *ext_entry_array,int line_number){
	char *token;
	token = strtok(NULL," \t");/*get the first token after .extern or .entry*/
	while(token){
		if(strcmp(directive,".extern") == 0){
			add_extern(ext_entry_array,token,line_number);/*add the extern to the extern array*/
		}
		else if(strcmp(directive,".entry") == 0){
			add_entry(ext_entry_array,token,line_number);/*add the entry to the entry array*/
		}
		token = strtok(NULL," \t");/*get the next token*/
	}
}

/*** DIRECTIVE HANDLING SECTION ***/

/* 
* handle_data_directive
* ---------------------
* this function processes the .data directive in the source code
* it extracts and validates the numerical values provided in the directive and increments the line counter accordingly
* this function is crucial for properly handling data declarations in the assembly process
* 
* parameters:
*   line: the line of code containing the .data directive
*   line_counter: a pointer to the line counter that tracks the current position in the source code
* 
*/
void handle_data_directive(char *line,int *line_counter){
	char *token;
	char *endptr;
	char *original_line = (char*)malloc(strlen(line) + 1);/*store original line for error messages*/
	char *line_copy;

	if(original_line == NULL){
		fprintf(stderr,"Error: Memory allocation failed for line copy\n");
		exit(EXIT_FAILURE);
	}

	strcpy(original_line,line);/*copy the original line*/
	line_copy = original_line;/*duplicate the line*/

	if(!validate_commas(line_copy,*line_counter,true)){/* Validate commas in the .data directive */
		free(original_line);
		exit(EXIT_FAILURE);/*exit if commas are invalid*/
	}

	token = strtok(line_copy," \t,");/*get the first token after .data*/
	while(token){
		strtol(token,&endptr,10);/*convert token to integer*/
		if(*endptr != '\0'){
			fprintf(stderr,"Error: Invalid number '%s' in .data directive on line %d\n",token,*line_counter);
			free(original_line);
			exit(EXIT_FAILURE);
		}
		else{
			(*line_counter)++;/*increase line counter for each data value*/
		}
		token = strtok(NULL," \t,");/*get the next token ignoring spaces tabs commas*/
	}

	free(original_line);/*free the duplicated line memory*/
}

/* 
* handle_string_directive
* -----------------------
* this function processes the .string directive in the source code
* it extracts the string value between quotes and increments the line counter for each character and the null terminator
* this function is essential for properly handling string declarations in the assembly process
* 
* parameters:
*   line: the line of code containing the .string directive
*   line_counter: a pointer to the line counter that tracks the current position in the source code
* 
*/
void handle_string_directive(char *line, int *line_counter){
	char *start;
	char *end;
	char *last_quote = NULL; /*to store the position of the last quote*/

	/*trim leading whitespace*/
	while(*line == ' ' || *line == '\t'){
		line++;
	}

	/*check for the opening quote*/
	if(*line != '"'){
		fprintf(stderr, "Error: Expected opening quote for string directive on line %d\n", *line_counter);
		exit(EXIT_FAILURE);
	}

	start = line + 1; /*move past the opening quote*/
	end = line + strlen(line) - 1; /*point to the last character of the line*/

	/*work backwards to find the last quote*/
	while(end > start && (*end == ' ' || *end == '\t')){
		end--; /*skip trailing whitespace*/
	}

	if(*end != '"'){
		fprintf(stderr, "Error: Missing closing quote or extraneous characters after string directive on line %d\n", *line_counter);
		exit(EXIT_FAILURE);
	}

	last_quote = end; /*set last_quote to the found quote*/
	end = last_quote - 1; /*move end to the character before the last quote*/

	/* Process the string between the quotes */
	while(start < last_quote){
		(*line_counter)++; /*increase line counter for each character in the string*/
		start++;
	}

	(*line_counter)++; /*increase line counter for the null terminator*/
}

/*** COMMA VALIDATION SECTION ***/

/* 
* validate_commas
* ---------------
* this function checks the correct usage of commas in a line of code
* it ensures that commas are placed correctly according to the rules for .data directives or general operand lists
* this function is important for catching syntax errors related to comma placement
* 
* parameters:
*   line: the line of code to validate
*   line_counter: the current line number being processed
*   is_data: a flag indicating whether the line is part of a .data directive
* 
* returns:
*   bool: true if the commas are valid false otherwise
*/
bool validate_commas(const char *line,int line_counter,bool is_data){
	int i;
	int expecting_number_or_operand = 1;/*initialize to expect a number or operand*/
	char prev_char = '\0';/*initialize to track previous character*/
	bool in_string = false;/*initialize to track if we're in a string*/

	for(i=0 ; line[i]!='\0' ; i++){
		char current_char = line[i];
		if(is_data){
			if(current_char == ','){
				if(expecting_number_or_operand || prev_char == ','){
					fprintf(stderr,"Error: Invalid comma placement in .data directive on line %d. Line: %s\n",line_counter,line);
					return false;
				}
				expecting_number_or_operand = 1;/*after a comma expect a number*/
			}
			else if(current_char != ' ' && current_char != '\t'){
				expecting_number_or_operand = 0;/*after a number expect a comma*/
			}
		}
		else{
			if(current_char == ',' && !in_string){
				if(expecting_number_or_operand || prev_char == ','){
					fprintf(stderr,"Error: Invalid comma placement on line %d. Line: %s\n",line_counter,line);
					return false;
				}
				expecting_number_or_operand = 1;/*after a comma expect an operand*/
			}
			else if(current_char == '"'){
				in_string = !in_string;/*toggle string status*/
			}
			else if(!in_string && current_char != ' ' && current_char != '\t'){
				expecting_number_or_operand = 0;/*after an operand expect a comma*/
			}
		}
		prev_char = current_char;
	}

	if(prev_char == ','){
		fprintf(stderr,"Error: Cannot have a trailing comma on line %d. Line: %s\n",line_counter,line);
		return false;
	}

	return true;/*commas are valid*/
}

/*** FIRST PASS MAIN FUNCTION ***/

/* 
* first_pass
* ----------
* this function performs the first pass over the input file
* it processes labels operations and directives collecting labels and preparing data for the second pass
* this function is critical as it sets up the necessary information needed to generate the final machine code during the second pass
* 
* parameters:
*   filename: the name of the source file to process
*   label_array: a pointer to the LabelArray structure to store labels found during the first pass
*   ext_entry_array: a pointer to the ExternEntryArray structure to store externs and entries found during the first pass
* 
* returns:
*   LabelArray*: a pointer to the LabelArray containing all labels found during the first pass or NULL if an error occurs
*/
LabelArray *first_pass(const char *filename, LabelArray *label_array, ExternEntryArray *ext_entry_array){
	FILE *file;
	char line[MAX_LINE_LENGTH + 1];
	int line_counter = IC_INIT_VALUE;/*initialize line counter with initial IC value*/
	char *token;
	int user_line_counter = 1;/*initialize user-friendly line counter*/
	char *colon;
	char *string_directive;

	file = fopen(filename, "r");/*open the input file*/
	if(!file){
		perror("Error opening file");
		return NULL;
	}

	while(fgets(line, sizeof(line), file)){/*read a line from the input file*/
		if(strlen(line) > MAX_LINE_LENGTH){
			fprintf(stderr, "Error: Line %d exceeds maximum length of %d characters\n", user_line_counter, MAX_LINE_LENGTH);
			exit(EXIT_FAILURE);
		}

		line[strcspn(line, "\r\n")] = 0;/*remove newline characters*/
		/*skip empty lines or lines with only whitespace*/
		if (strlen(line) == 0 || strspn(line, " \t") == strlen(line)) {
			user_line_counter++; /*increase the user line counter*/
			continue;
		}
		/*check for various comma related errors*/
		if(strstr(line, ".data") != NULL){
			if(!validate_commas(line, line_counter, true)){/*validate .data directive commas*/
				exit(EXIT_FAILURE);
			}
		}
		else{
			if(!validate_commas(line, line_counter, false)){/*validate general operand commas*/
				exit(EXIT_FAILURE);
			}
		}

		/*check for labels*/
		token = strtok(line, " \t");
		if(token && strchr(token, ':')){
			colon = strchr(token, ':');
			*colon = '\0';/*remove the colon*/
			if(is_valid_label(token)){
				add_label(label_array, token, line_counter);/*add the label to the label array*/
			}
			else{
				fprintf(stderr, "Error: Invalid label '%s' on line %d\n", token, user_line_counter);
				exit(EXIT_FAILURE);
			}
			token = strtok(NULL, " \t");/*get the next token after the label*/
		}

		/*handle the remaining part of the line*/
		if(token == NULL){
			user_line_counter++;
			line_counter++;/*if there is no more token after label, increase line counter for lines that only have labels*/
			continue;
		}

		if(is_operation(token)){
			int num_of_lines;/*declare variable at the start*/
			char *operands = strtok(NULL, "\n");/*get the remaining part of the line as operands*/
			num_of_lines = calculate_instruction_lines(token, operands);/*calculate the number of lines needed for the operation*/
			line_counter += num_of_lines;/*add the required lines to the line counter*/
		}
		else if(strcmp(token, ".data") == 0){
			char *data_part = strtok(NULL, "\n");/*get the remaining part of the line after .data*/
			if(data_part != NULL){
				handle_data_directive(data_part, &line_counter);/*handle the .data directive*/
			}
			else{
				fprintf(stderr, "Error: Invalid .data directive on line %d\n", user_line_counter);
				exit(EXIT_FAILURE);
			}
		}
		else if(strcmp(token, ".string") == 0){
			string_directive = strtok(NULL, "\n");/*get the rest of the line after .string*/
			if(string_directive != NULL){
				while(*string_directive == ' ' || *string_directive == '\t'){
					string_directive++;/*skip any leading whitespace*/
				}
				handle_string_directive(string_directive, &line_counter);/*handle the .string directive*/
			}
			else{
				fprintf(stderr, "Error: Invalid string directive on line %d\n", user_line_counter);
				exit(EXIT_FAILURE);
			}
		}
		else if(strcmp(token, ".extern") == 0 || strcmp(token, ".entry") == 0){
			handle_extern_entry_directive(token, line, ext_entry_array, line_counter);/*handle .extern and .entry directives*/
		}
		else{
			fprintf(stderr, "Error: Unknown directive or operation '%s' on line %d\n", token, user_line_counter);
			exit(EXIT_FAILURE);
		}
		/* ccheck for RAM overflow*/
		if(line_counter > CODE_ARR_IMG_LENGTH) {
			fprintf(stderr, "Error: RAM overflow\n");
			exit(EXIT_FAILURE);
		}
		user_line_counter++; /* Increase the user line counter */
	}

	fclose(file);
	return label_array;/*return the pointer to the label array*/
}

