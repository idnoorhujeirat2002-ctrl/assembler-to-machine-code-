#ifndef _FIRST_PASS_H
#define _FIRST_PASS_H

#include "definitions.h"/*include definitions header file*/
#include "utils.h"/*include utils header file*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_OPERATIONS (sizeof(operations)/sizeof(operations[0]))/*calculate the number of operations*/
#define MAX_LABEL_LENGTH 31/*maximum length for a label*/

/*** STRUCTURE DEFINITIONS SECTION ***/

typedef struct{/*label struct that has a string of the label name and its line*/ 
	char label[MAX_LABEL_LENGTH];/*label name*/ 
	int line_number;/*line number*/ 
}Label;

typedef struct{/*array that stores the labels with the number of labels and the capacity of how much it can hold for the dynamic allocation*/ 
	Label *labels;/*pointer to the array of labels*/ 
	int count;/*number of labels*/ 
	int capacity;/*capacity for dynamic allocation*/ 
}LabelArray;

typedef struct{/*structure to store extern or entry data (name and line number)*/ 
	char name[MAX_LABEL_LENGTH];/*extern/entry name*/ 
	int line_number;/*line number*/ 
}ExternEntry;

typedef struct{/*array that stores the extern and entry with the number of extern and entry variables and the capacity of how much it can hold for the dynamic allocation*/ 
	ExternEntry externs[MAX_EXTERN_ENTRIES];/*array of externs*/ 
	ExternEntry entries[MAX_EXTERN_ENTRIES];/*array of entries*/ 
	int extern_count;/*count of externs*/ 
	int entry_count;/*count of entries*/ 
}ExternEntryArray;

typedef struct{/*structure to hold information about an operation and its valid addressing types*/ 
	const char *operation;/*operation name*/ 
	int valid_src_types[5];/*valid source addressing types*/ 
	int valid_dest_types[5];/*valid destination addressing types*/ 
}operation_info;

/*** OPERATION HANDLING SECTION ***/

bool is_operation(const char *token);/*function to check if the token is a valid operation*/
int get_operand_count(const char *operation);/*function to return the number of operands that the operation takes*/

/*** LABEL HANDLING SECTION ***/

bool is_unique_label(LabelArray *array,const char *label);/*function to check if the label is not already initialized*/
void initialize_label_array(LabelArray *array);/*function to initialize the LabelArray structure*/
void add_label(LabelArray *array,const char *label,int line_number);/*function to add a label to the LabelArray*/
bool is_valid_label(const char *label);/*function to check if a string is a valid label*/

/*** EXTERN AND ENTRY HANDLING SECTION ***/

void initialize_extern_entry_array(ExternEntryArray *array);/*function to initialize the ExternEntryArray structure*/
void add_extern(ExternEntryArray *array,const char *name,int line_number);/*function to add an extern variable*/
void add_entry(ExternEntryArray *array,const char *name,int line_number);/*function to add an entry variable*/
void handle_extern_entry_directive(char *directive,char *line,ExternEntryArray *ext_entry_array,int line_number);/*function to handle .extern and .entry directives*/

/*** DIRECTIVE HANDLING SECTION ***/

void handle_data_directive(char *line,int *line_counter);/*function to handle .data directive*/
void handle_string_directive(char *line,int *line_counter);/*function to handle .string directive*/

/*** FIRST PASS MAIN FUNCTION ***/

LabelArray* first_pass(const char *filename, LabelArray *label_array, ExternEntryArray *ext_entry_array);/*main function for the first pass*/
bool validate_commas(const char *line,int line_counter,bool is_data);/*function to validate commas in the line*/

#endif/*_FIRST_PASS_H*/

