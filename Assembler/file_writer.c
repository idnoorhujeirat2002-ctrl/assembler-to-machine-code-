#include"file_writer.h"

/* 
 * convert_code_word_to_binary
 * ---------------------------
 * this function converts a code word structure to its corresponding 15-bit binary representation
 * it shifts and combines the opcode source address destination address and are fields into a single binary value
 * 
 * parameters:
 *   codeword: a pointer to the codeword structure that needs to be converted
 * 
 * returns:
 *   the 15-bit binary representation of the code word
 */
unsigned int convert_code_word_to_binary(codeWord*codeword){/*convert code word to binary*/
	unsigned int binary_value = 0;/*initialize binary value*/

	/*shift and combine the fields*/
	binary_value |= (codeword->opcode << 11);/*shift opcode 11 bits to the left and combine with binary value*/
	binary_value |= (codeword->src_address << 7);/*shift source address 7 bits to the left and combine with binary value*/
	binary_value |= (codeword->dest_address << 3);/*shift destination address 3 bits to the left and combine with binary value*/
	binary_value |= codeword->ARE;/*combine are directly with binary value*/

	return binary_value;/*return the 15-bit binary value*/
}

/* 
 * handle_data_value
 * -----------------
 * this function handles data values including negative values by converting them to a 15-bit binary number
 * for negative numbers it converts them to their twos complement 15-bit representation
 * 
 * parameters:
 *   data_value: the data value (integer) that needs to be converted to a 15-bit binary number
 * 
 * returns:
 *   the 15-bit binary representation of the data value
 */
unsigned int handle_data_value(int data_value){/*handle data values considering negative values*/
	unsigned int binary_value = 0;/*initialize binary value*/

	if(data_value < 0){/*check if the data value is negative*/
		data_value = (1 << 15 ) +data_value;/*convert to two's complement 15-bit value*/
	}

	binary_value = (data_value&0x7FFF);/*ensure the value fits in 15 bits by applying a 15-bit mask*/

	return binary_value;/*return the 15-bit binary value*/
}

/* 
 * convert_data_word_to_binary
 * ---------------------------
 * this function converts a data word structure to its corresponding 15-bit binary representation
 * it shifts and combines the data and are fields into a single binary value
 * 
 * parameters:
 *   dataword: a pointer to the dataword structure that needs to be converted
 * 
 * returns:
 *   the 15-bit binary representation of the data word
 */
unsigned int convert_data_word_to_binary(dataWord*dataword){/*convert data word to binary*/
	unsigned int binary_value = 0;/*initialize binary value*/

	/*shift and combine the fields*/
	binary_value |= ((dataword->data&0xFFF) << 3);/*shift the 12 bits of data 3 bits to the left and combine with binary value*/
	binary_value |= dataword->ARE;/*combine are directly with binary value*/

	return binary_value;/*return the 15-bit binary value*/
}

/* 
 * convert_binary_to_octal
 * -----------------------
 * this function converts a 15-bit binary number directly to its octal representation
 * it iterates through the binary number converting each set of three bits to an octal digit
 * 
 * parameters:
 *   binary_value: the binary value to be converted to octal
 * 
 * returns:
 *   the octal representation of the binary value
 */
unsigned int convert_binary_to_octal(unsigned int binary_value){/*convert a binary number directly to octal*/
	unsigned int octal_value = 0, place_value = 1;/*initialize octal value and place value*/

	while(binary_value != 0){/*loop until binary value is zero*/
		octal_value += (binary_value%8)*place_value;/*calculate the current octal digit and add it to the octal value*/
		binary_value /= 8;/*divide binary value by 8 to move to the next digit*/
		place_value *= 10;/*update place value by multiplying it by 10*/
	}
	return octal_value;/*return the final octal value*/
}

/* 
 * write_ob_file
 * -------------
 * this function writes the machine code (both code and data segments) to an output file in the specified format
 * it converts each word in the code and data segments to its binary representation then to octal and writes them to the file
 * 
 * parameters:
 *   code_img: array of pointers to machinewordsarray structures representing the code segment
 *   data_img: array of long integers representing the data segment
 *   icf: the final value of the instruction counter after the first pass
 *   dcf: the final value of the data counter after the first pass
 *   filename: the base name of the file to which the output will be written
 */
void write_ob_file(MachineWordsArray**code_img,long*data_img,long icf,long dcf,const char*filename){/*write the converted words to a file*/
	FILE *file;/*declare a file pointer*/
	int i;/*declare a loop counter*/
	unsigned int binary_value, octal_value;/*declare variables for binary and octal values*/
	char output_filename[MAX_FILENAME_LENGTH + 5];/*declare a string to hold the output filename with .ob suffix*/

	/*construct the output filename with .ob suffix*/
	sprintf(output_filename, "%s.ob", filename);

	file = fopen(output_filename, "w");/*open the file for writing*/
	if(!file){/*check if the file failed to open*/
		perror("error opening file for writing");/*print an error message*/
		exit(EXIT_FAILURE);/*exit the program with failure status*/
	}

	/*write header with ic and dc values*/
	fprintf(file, "%ld %ld\n", icf - IC_INIT_VALUE, dcf);

	/*process code image*/
	for(i=0 ; i<icf-IC_INIT_VALUE ; i++){
		if(code_img[i]->length > 0){/*check if the code word is present*/
			binary_value = convert_code_word_to_binary(code_img[i]->word.code);/*convert code word to binary*/
		}
		else{/*if data word is present*/
			binary_value = convert_data_word_to_binary(code_img[i]->word.data);/*convert data word to binary*/
		}

		octal_value = convert_binary_to_octal(binary_value);/*convert binary value directly to octal*/

		/*write to file using correct format specifiers*/
		fprintf(file, "%.4d %.5d\n", i + 100, octal_value);
	}

	/*process data image*/
	for(i=0 ; i<dcf ; i++){
		binary_value=handle_data_value(data_img[i]);/*convert data value directly to a 15-bit binary number*/

		octal_value=convert_binary_to_octal(binary_value);/*convert binary value directly to octal*/

		/*write to file using correct format specifiers*/
		fprintf(file,"%.4d %.5d\n",(int)(i+100+icf-IC_INIT_VALUE),octal_value);
	}

	fclose(file);/*close the file*/
}

/* 
 * write_entries_to_file
 * ---------------------
 * this function writes the entry labels and their line numbers to an output file in reverse order
 * the entries are written only if they exist otherwise the function does nothing
 * 
 * parameters:
 *   filename: the base name of the file to which the entries will be written
 *   ext_entry_array: array of externentry structures containing the entries to be written
 */
void write_entries_to_file(const char*filename,ExternEntryArray*ext_entry_array){/*write the entry labels and their line numbers to a file*/
	FILE *file;/*declare a file pointer*/
	int i;/*declare a loop counter*/
	bool has_entries = false;/*flag to check if there are any entries*/
	char output_filename[MAX_FILENAME_LENGTH + 5];/*declare a string to hold the output filename with .ent suffix*/

	for(i=0 ; i<ext_entry_array->entry_count ; i++){/*check if there are any entries*/
		has_entries = true;/*set the flag to true if an entry is found*/
		break;/*break the loop after finding the first entry*/
	}

	if(!has_entries){/*if no entries are found*/
		return;/*do not create a file and return*/
	}

	sprintf(output_filename, "%s.ent", filename);/*construct the output filename with .ent suffix*/

	file=fopen(output_filename,"w");/*open the file for writing*/
	if(!file){/*check if the file failed to open*/
		perror("error opening file for writing");/*print an error message*/
		exit(EXIT_FAILURE);/*exit the program with failure status*/
	}

	for(i=ext_entry_array->entry_count-1 ; i>=0 ; i--){/*write each entry label and its line number in reverse order*/
		fprintf(file,"%s %04d\n",ext_entry_array->entries[i].name, ext_entry_array->entries[i].line_number);/*write the label and line number*/
	}

	fclose(file);/*close the file*/
}

/* 
 * write_externs_to_file
 * ---------------------
 * this function writes the extern labels and their line numbers to an output file
 * the externs are written only if they exist otherwise the function does nothing
 * 
 * parameters:
 *   filename: the base name of the file to which the externs will be written
 *   extern_printing_array: array of externentry structures containing the externs to be written
 */
void write_externs_to_file(const char*filename,ExternEntry*extern_printing_array){/*write the extern labels and their line numbers to a file*/
	FILE*file;/*declare a file pointer*/
	int i = 0;/*declare a loop counter*/
	bool has_externs = false;/*flag to check if there are any externs*/
	char output_filename[MAX_FILENAME_LENGTH + 5];/*declare a string to hold the output filename with .ext suffix*/

	while(extern_printing_array[i].name[0] != '\0'){/*check if there are any externs*/
		has_externs = true;/*set the flag to true if an extern is found*/
		break;/*break the loop after finding the first extern*/
	}

	if(!has_externs){/*if no externs are found*/
		return;/*do not create a file and return*/
	}

	sprintf(output_filename, "%s.ext", filename);/*construct the output filename with .ext suffix*/

	file = fopen(output_filename, "w");/*open the file for writing*/
	if(!file){/*check if the file failed to open*/
		perror("error opening file for writing");/*print an error message*/
		exit(EXIT_FAILURE);/*exit the program with failure status*/
	}

	while(extern_printing_array[i].name[0] != '\0'){/*write each extern label and its line number*/
		fprintf(file,"%s %04d\n", extern_printing_array[i].name, extern_printing_array[i].line_number);/*write the label and line number*/
		i++;/*move to the next extern*/
	}

	fclose(file);/*close the file*/
}

