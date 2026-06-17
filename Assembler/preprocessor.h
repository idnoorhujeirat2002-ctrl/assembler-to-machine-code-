#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definitions.h"

#define INVALID_NAME_COUNT (sizeof(invalid_names)/sizeof(invalid_names[0]))/*define the list of invalid macro names*/
#define INITIAL_MACRO_COUNT 10/*initial count for macros in the array*/

/*** STRUCTURE DEFINITIONS SECTION ***/

/*typedef for macro structure*/
typedef struct {
	char *name;/*name of the macro*/
	char **content;/*array of lines in the macro*/
	int line_count;/*number of lines in the macro*/
}Macro;

/*typedef for macro array structure*/
typedef struct {
	Macro *macros;/*dynamically allocated array of macros*/
	int count;/*current number of macros*/
	int capacity;/*current capacity of the array*/
}MacroArray;

/*** EXTERNAL DECLARATIONS SECTION ***/

/*invalid names that a macro can't have*/
extern const char *invalid_names[];

/*** FUNCTION PROTOTYPES SECTION ***/

void initialize_macro_array(MacroArray *array);/*function to initialize the MacroArray structure*/
void add_macro(MacroArray *array,const char *name,char **content,int line_count);/*function to add a macro to the MacroArray*/
char **get_macro_content(MacroArray *array,const char *name,int *line_count);/*function to retrieve the content of a macro*/
void free_macro_array(MacroArray *array);/*function to free all macros in the MacroArray*/
bool preprocess_file(const char *input_filename);/*function to preprocess the file and expand macros*/
bool is_valid_macro_name(const char *name);/*function to check if a macro name is valid*/
char *allocate_and_copy_string(const char *s);/*function to allocate memory and copy a string*/

#endif /*PREPROCESSOR_H*/

