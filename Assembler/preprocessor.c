#include "preprocessor.h"

/*** INVALID NAMES SECTION ***/

/*
* invalid_names
* -------------
* an array of strings representing invalid macro names
* this array includes the names of operations and registers that cannot be used as macro names
* by storing these in a central array it allows for easy checking when validating macro names
*/
const char *invalid_names[] = {
	"mov","cmp","add","sub","lea","clr","not","inc",
	"dec","jmp","bne","red","prn","jsr","rts","stop",
	"r0","r1","r2","r3","r4","r5","r6","r7"
};

/*** MACRO ARRAY HANDLING SECTION ***/

/* 
* initialize_macro_array
* ----------------------
* this function initializes a MacroArray by allocating memory for the initial array of macros
* it also sets the initial count and capacity of the array
* initializing the macro array is necessary before any macros can be added to it
* 
* parameters:
*   array: a pointer to the MacroArray structure that will be initialized
* 
*/
void initialize_macro_array(MacroArray *array){
	array->macros=(Macro*)malloc(sizeof(Macro)*INITIAL_MACRO_COUNT);/*allocate initial memory for the macros array*/
	array->count=0;/*initialize macro count to 0*/
	array->capacity=INITIAL_MACRO_COUNT;/*set the initial capacity*/
}

/* 
* add_macro
* ---------
* this function adds a new macro to the MacroArray
* if the array has reached its capacity it reallocates memory to accommodate more macros
* it then stores the macros name content and the number of lines it contains in the array
* this function ensures that macros are stored dynamically as the array expands
* 
* parameters:
*   array: a pointer to the MacroArray structure where the macro will be added
*   name: the name of the macro which must be unique and valid
*   content: an array of strings representing the lines of code within the macro
*   line_count: the number of lines in the macro content array
* 
*/
void add_macro(MacroArray *array,const char *name,char **content,int line_count){
	if(array->count>=array->capacity){/*if capacity is too low, double it and reallocate memory*/
		array->capacity*=2;/*double the capacity*/
		array->macros=(Macro*)realloc(array->macros,sizeof(Macro)*array->capacity);/*reallocate memory for the expanded array*/
		if(array->macros==NULL){/*ensure realloc succeeded*/
			perror("realloc failed");
			exit(EXIT_FAILURE);
		}
	}
	array->macros[array->count].name=allocate_and_copy_string(name);/*store the macro name*/
	array->macros[array->count].content=content;/*store the macro content*/
	array->macros[array->count].line_count=line_count;/*store the number of lines in the macro*/
	array->count++;/*increment the count of macros in the array*/
}

/* 
* get_macro_content
* -----------------
* this function retrieves the content of a macro by its name from the MacroArray
* it also provides the number of lines in the macros content
* this function is used during preprocessing to expand macros into their full content in the output file
* 
* parameters:
*   array: a pointer to the MacroArray structure that contains all macros
*   name: the name of the macro to retrieve
*   line_count: a pointer to an integer where the number of lines in the macro will be stored
* 
* returns:
*   char**: a pointer to the array of strings representing the macro's content or NULL if the macro is not found
*/
char **get_macro_content(MacroArray *array, const char *name, int *line_count) {
    int i;
    for (i = 0; i < array->count; i++) {
        if (strcmp(array->macros[i].name, name) == 0) { /* Correctly compare strings */
            *line_count = array->macros[i].line_count; /* set the line count to the macro's line count */
            return array->macros[i].content; /* return the content of the macro */
        }
    }
    return NULL; /* return NULL if no matching macro is found */
}


/* 
* free_macro_array
* ----------------
* this function frees all the memory allocated for the macros in the MacroArray
* it iterates through each macro freeing the memory for the name content and then the array itself
* this function ensures that no memory leaks occur by properly cleaning up all allocated resources
* 
* parameters:
*   array: a pointer to the MacroArray structure to be freed
* 
*/
void free_macro_array(MacroArray *array){
	int i,j;
	for(i=0 ; i<array->count ; i++){/*iterate through all the macros in the array*/
		free(array->macros[i].name);/*free the memory allocated for the macro name*/
		for(j=0;j<array->macros[i].line_count;j++){/*iterate through the macro's content*/
			free(array->macros[i].content[j]);/*free each line of the macro's content*/
		}
		free(array->macros[i].content);/*free the macro content pointer*/
	}
	free(array->macros);/*free the macro array itself*/
}

/*** MACRO NAME VALIDATION SECTION ***/

/* 
* is_valid_macro_name
* -------------------
* this function checks if a given macro name is valid by comparing it against a list of invalid names
* it ensures that the macro name does not conflict with operation names or register names
* this function is crucial for preventing errors during preprocessing by enforcing naming rules
* 
* parameters:
*   name: the name of the macro to validate
* 
* returns:
*   bool: true if the name is valid and false if it is not
*/
bool is_valid_macro_name(const char *name){
	int i;
	for(i=0;i<INVALID_NAME_COUNT;++i){
		if(strcmp(name,invalid_names[i])==false){/*if the name matches any of the invalid names*/
			return false;/*return false to indicate the name is invalid*/
		}
	}
	return true;/*return true if the name is valid*/
}

/* 
* allocate_and_copy_string
* ------------------------
* this function allocates memory for a string and copies the provided string into the allocated memory
* it is used to safely store strings that are dynamically created during preprocessing
* this function ensures that strings are properly managed in memory preventing errors from incorrect memory handling
* 
* parameters:
*   s: the string to be copied
* 
* returns:
*   char*: a pointer to the newly allocated and copied string
*/
char *allocate_and_copy_string(const char *s){
	char *d=malloc(strlen(s)+1);/*allocate memory for the string*/
	if(d==NULL){/*ensure malloc succeeds*/
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}
	strcpy(d,s);/*copy the string into the allocated memory*/
	return d;/*return the pointer to the copied string*/
}

/*** FILE PREPROCESSING SECTION ***/

/* 
* preprocess_file
* ---------------
* this function preprocesses an assembly source file by expanding macros and writing the result to a new file with a .am extension
* it reads the input file line by line checking for macro definitions and expanding any macros that are called
* the function handles the creation and management of macros including their storage retrieval and expansion
* it also manages the input and output files ensuring that the preprocessed file is correctly generated
* 
* parameters:
*   input_filename: the name of the assembly source file to preprocess
* 
* returns:
*   bool: true if the preprocessing was successful and false if an error occurred
*/
bool preprocess_file(const char *input_filename){
	FILE *input_file;
	FILE *output_file;

	MacroArray macros;/*declare a MacroArray to store all macros*/
	char line[MAX_LINE_LENGTH];/*buffer to hold each line of the input file*/
	int in_macro = false;/*flag to indicate if we are inside a macro definition*/
	char macro_name[MAX_LINE_LENGTH];/*buffer to hold the name of the macro being defined*/
	char **macro_content = NULL;/*pointer to hold the content of the macro being defined*/
	int macro_lines = 0;/*number of lines in the macro*/
	int macro_capacity = 0;/*capacity of the macro content array*/
	char output_line[MAX_LINE_LENGTH];/*buffer for the line to be written to the output file*/
	char word[MAX_LINE_LENGTH];/*buffer to hold a word extracted from the line*/
	int word_len;/*length of the extracted word*/
	int macro_line_count;/*number of lines in the macro being retrieved*/
	char **macro_def;/*pointer to the content of the retrieved macro*/
	int i;/*loop variable*/
	char *start;/*initialize pointer*/
	char *colon;/*pointer to locate the colon in the label*/
	char output_filename[MAX_FILENAME_LENGTH];/*buffer to store the output filename*/
	char *dot = strrchr(input_filename, '.');/*find the last occurrence of a dot in the input filename*/

	/*check if the file has a .as extension*/
	if(dot != NULL && strcmp(dot, ".as") == false){
		strncpy(output_filename, input_filename, dot - input_filename);/*copy the input filename up to the dot into the output filename buffer*/
		output_filename[dot - input_filename] = '\0';/*terminate the string in the output filename buffer*/
		strcat(output_filename, ".am");/*append the .am extension to the output filename*/
	}
	else{
		fprintf(stderr, "Invalid input file extension. Expected .as\n");
		return false;/*return false if the input file doesn't have a .as extension*/
	}

	input_file = fopen(input_filename, "r");
	output_file = fopen(output_filename, "w");
	if(!input_file){
		perror("Error opening input file");
		return false;
	}
	if(!output_file){
		perror("Error opening output file");
		fclose(input_file);
		return false;
	}

	initialize_macro_array(&macros);/*initialize the macro array*/

	while(fgets(line, sizeof(line), input_file)){/*read a line from the input file*/
		if(line[0] == ';'){/*skip comment lines*/
			continue;
		}

		/* Move the pointer to the first non-space character */
		start = line;
		while(*start == ' ' || *start == '\t') {
			start++;
		}

		if(strncmp(start, "macr ", 5) == false){/*check if the line starts with "macr "*/
			char *macro_def_check = start + 5;/*set macro_def_check to point to the character after "macr "*/
			while(*macro_def_check == ' ') macro_def_check++;/*skip any spaces after "macr "*/
			sscanf(macro_def_check, "%s", macro_name);/*read the macro name*/

			if(!is_valid_macro_name(macro_name)){/*check if the macro name is valid*/
				fprintf(stderr, "Invalid macro name: %s\n", macro_name);
				fclose(input_file);
				fclose(output_file);
				free_macro_array(&macros);
				return false;/*macro name is invalid*/
			}

			in_macro = true;/*set the in_macro flag to true indicating that we are now inside a macro definition*/
			macro_capacity = 10;/*set the initial capacity for macro lines*/
			macro_lines = 0; /*initialize the line count to 0*/
			macro_content = (char **)malloc(sizeof(char *) * macro_capacity);/*allocate memory for macro lines*/
			if(macro_content == NULL){
				perror("malloc failed");
				exit(EXIT_FAILURE);
			}
			continue;
		}

		if(strncmp(start, "endmacr", 7) == false){ /*check if the line starts with "endmacr"*/
			add_macro(&macros, macro_name, macro_content, macro_lines);/*add the macro to the macro array*/
			in_macro = false;/*set the in_macro flag to false, indicating that we are no longer inside a macro definition*/
			macro_content = NULL;/*reset macro_content pointer to NULL*/
			continue;
		}

		if(in_macro){/*if we are inside a macro definition*/
			if(macro_lines >= macro_capacity){/*check if the macro content array needs to be resized*/
				macro_capacity *= 2;/*double the capacity*/
				macro_content = (char **)realloc(macro_content, sizeof(char *)*macro_capacity);/*reallocate memory for the expanded array*/
				if(macro_content == NULL){/*ensure realloc succeeds*/
					perror("realloc failed");
					exit(EXIT_FAILURE);
				}
			}
			macro_content[macro_lines++] = allocate_and_copy_string(start);/*copy the line to macro content*/
			continue;
		}

		output_line[0] = '\0';/*initialize the output line buffer to an empty string*/

		colon = strchr(start, ':');
		if(colon){
			*colon = '\0';
			strcat(output_line, start);
			strcat(output_line, ": ");
			start = colon + 1;
			/* Skip any additional spaces after the colon */
			while(*start == ' ' || *start == '\t') {
				start++;
			}
		}

		while(*start){
			sscanf(start, "%s", word);/*read a word from the line*/
			word_len = strlen(word);/*get the length of the word*/
			macro_def = get_macro_content(&macros, word, &macro_line_count);/*get the content of the macro*/

			if(macro_def){/*if the word matches a macro name*/
				for(i=0 ; i<macro_line_count ; i++){/*iterate through the macro lines*/
					strcat(output_line, macro_def[i]);/*append each line of the macro to the output line*/
				}
				start += word_len;
				while(*start && (*start==' ' || *start=='\t' || *start=='\n' || *start=='\r')){
					start++;
				}
				break;/*after macro expansion, skip the rest of the line*/
			}
			else{
				strncat(output_line, start, 1);/*preserve spaces and tabs*/
				start++;
			}
		}

		fputs(output_line, output_file);/*write the output line to the file*/
	}

	fclose(input_file);/*close the input file*/
	fclose(output_file);/*close the output file*/
	free_macro_array(&macros);/*free the memory allocated for macros*/
	return true;/*return true to indicate successful preprocessing*/
}


