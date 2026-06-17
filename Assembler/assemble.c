#include "preprocessor.h"
#include "first_pass.h"
#include "second_pass.h"
#include "file_writer.h"
#include "definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
* delete_file
* -----------
* this function deletes a specified file from the filesystem
* it attempts to remove the file and prints an error message if the operation fails
* this function is useful for cleaning up temporary files or failed builds
* 
* parameters:
*   filename: the name of the file to be deleted
* 
*/
void delete_file(const char *filename){
	if(remove(filename) != 0){
		perror("Error: couldn't delete file");/*print an error message if file deletion fails*/
	}
}

/*
* update_entry_lines
* ------------------
* this function updates the line numbers in the ExternEntryArray based on the LabelArray
* it iterates through each entry and matches it with a corresponding label to set the correct line number
* this function is crucial for resolving extern and entry references to the correct locations in the assembly process
* 
* parameters:
*   label_array: a pointer to the LabelArray containing all labels
*   ext_entry_array: a pointer to the ExternEntryArray containing all extern and entry entries
* 
*/
void update_entry_lines(LabelArray *label_array, ExternEntryArray *ext_entry_array){
	int i, j;
	bool found;

	/*iterate through each entry in the ExternEntryArray*/
	for(i = 0; i < ext_entry_array->entry_count; i++) {
		found = false;/*initialize the found flag to false*/

		/*iterate through the LabelArray to find a matching label*/
		for(j = 0; j < label_array->count; j++) {
			if(strcmp(ext_entry_array->entries[i].name, label_array->labels[j].label) == 0) {
				/*if a match is found update the line number of the entry*/
				ext_entry_array->entries[i].line_number = label_array->labels[j].line_number;
				found = true;/*set found flag to true*/
				break;/*exit loop if a matching label is found*/
			}
		}

		/*if no matching label is found print an error message and exit*/
		if(!found) {
			fprintf(stderr, "Error: Entry label '%s' not found in label array\n", ext_entry_array->entries[i].name);
			exit(EXIT_FAILURE);/*terminate the program if the label is not found*/
		}
	}
}

/*
* process_file
* ------------
* this function processes a single assembly file through preprocessing, first pass, and second pass
* it manages the creation of intermediate files, handles errors, and generates the final output files
* this function is the main driver for processing each file and is called for each input file in the main function
* 
* parameters:
*   basename: the base name of the file to be processed (without extension)
* 
*/
void process_file(const char *basename) {
	char input_file[MAX_FILENAME_LENGTH];/*declare a string to store the input filename*/
	char preprocessed_file[MAX_FILENAME_LENGTH];/*declare a string to store the preprocessed filename*/
	LabelArray label_array;/*declare a LabelArray to store labels encountered during the first pass*/
	ExternEntryArray ext_entry_array;/*declare an ExternEntryArray to store extern and entry entries*/
	ExternEntry *extern_printing_array = NULL;/*initialize a pointer to ExternEntry array to NULL*/
	MachineWordsArray *code_img[CODE_ARR_IMG_LENGTH] = {0};/*initialize an array of MachineWordsArray pointers with NULL*/
	long IC = IC_INIT_VALUE;/*initialize the instruction counter (IC) with the initial value*/
	long DC = 0;/*initialize the data counter (DC) to 0*/
	long data_img[CODE_ARR_IMG_LENGTH];/*declare an array to store data image*/
	bool success;/*declare a boolean variable to store the success status of operations*/

	/*prepare filenames*/
	sprintf(input_file, "%s.as", basename);/*format the input filename by appending ".as" to the basename*/
	sprintf(preprocessed_file, "%s.am", basename);/*format the preprocessed filename by appending ".am" to the basename*/

	/*preprocess the file*/
	success = preprocess_file(input_file);/*call the preprocess_file function to preprocess the input file*/
	if(!success){/*check if preprocessing failed*/
		fprintf(stderr, "Error: preprocessing failed for file %s\n", input_file);
		delete_file(preprocessed_file);/*delete the .am file if preprocessing fails*/
		return;
	}

	/*initialize arrays*/
	initialize_label_array(&label_array);/*initialize the label array to store labels*/
	initialize_extern_entry_array(&ext_entry_array);/*initialize the extern entry array to store extern and entry entries*/

	/*first pass*/
	if(first_pass(preprocessed_file, &label_array, &ext_entry_array) == NULL){/*perform the first pass and check if it failed*/
		fprintf(stderr, "error: first pass failed for file %s\n", preprocessed_file);
		delete_file(preprocessed_file);/*delete the .am file if the first pass fails*/
		return;
	}

	/*update entry lines*/
	if(label_array.count > 0 && ext_entry_array.entry_count > 0) { /*check if there are labels and entries*/
		update_entry_lines(&label_array, &ext_entry_array);/*update the entry lines with correct label references*/
	}

	/*second pass*/
	success = second_pass(preprocessed_file, &label_array, &ext_entry_array, code_img, data_img, &IC, &DC, &extern_printing_array);/*perform the second pass*/
	if(!success) { /*check if the second pass failed*/
		fprintf(stderr, "error: second pass failed for file %s\n", preprocessed_file);/*print an error message*/
		delete_file(preprocessed_file);/*delete the .am file if the second pass fails*/
		return;/*exit the function*/
	}

	/*write output files*/
	write_ob_file(code_img, data_img, IC, DC, basename);/*write the object file (.ob) with the generated code and data*/

	/*conditionally write entry and extern files*/
	if(ext_entry_array.entry_count > 0) { /*check if there are any entries*/
		write_entries_to_file(basename, &ext_entry_array);/*write the entries to the .ent file*/
	}
	if(extern_printing_array != NULL) { /*check if there are any extern references*/
		write_externs_to_file(basename, extern_printing_array);/*write the externs to the .ext file*/
	}
	if(success){
		printf("Assembler succeeded for file %s\n",basename);
	}
	/*delete the .am file*/
	delete_file(preprocessed_file);/*delete the preprocessed file (.am) after processing is complete*/
}

/*
* main
* ----
* this function is the entry point of the program
* it processes command-line arguments and calls process_file for each input file
* 
* parameters:
*   argc: the number of command-line arguments
*   argv: an array of strings representing the command-line arguments
* 
* returns:
*   int: 0 if the program runs successfully or a non-zero value if an error occurs
*/
int main(int argc, char *argv[]) {
	int i;/*declare a loop counter*/
	char input_filename[MAX_FILENAME_LENGTH];/*buffer to hold the file name with .as suffix*/
	FILE *file;

	if(argc < 2) { /*check if at least one input file is provided*/
		fprintf(stderr, "usage: %s <file1> <file2> ... <fileN>\n", argv[0]);/*print usage instructions*/
		return false;
	}

	for(i=1 ; i<argc ; i++){/*loop through each input file argument*/
		sprintf(input_filename, "%s.as", argv[i]);/*create the input file name by adding the .as suffix to the argument*/

		/*check if the file exists*/
		file = fopen(input_filename, "r");
		if(file){
			fclose(file);/*close the file as it exists*/
			process_file(argv[i]);/*call process_file for the valid file*/
		}
		else{
			fprintf(stderr, "Warning: file %s.as does not exist skipping it\n", argv[i]);/*print warning if file doesn't have .as suffix*/
		}
	}
	return true;
}
