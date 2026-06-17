#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"definitions.h"
#include"first_pass.h"
#include"second_pass.h"


/*** WRITING FILES FUCNTIONS ***/
void write_entries_to_file(const char *filename, ExternEntryArray *ext_entry_array);/*function to write entries to the specified file*/
void write_externs_to_file(const char *filename, ExternEntry *extern_printing_array);/*function to write externs to the specified file*/
void write_ob_file(MachineWordsArray **code_img, long *data_img, long icf, long dcf, const char *filename);/*function to write the object file with the final machine code and data*/



#endif /* FILE_WRITER_H */
