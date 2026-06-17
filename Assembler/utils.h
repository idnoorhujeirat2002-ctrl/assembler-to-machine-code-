#ifndef UTILS_H
#define UTILS_H

#include "definitions.h"
#include <string.h>

/*** UTILITY FUNCTIONS SECTION ***/

bool is_alpha(char c);/*check if a character is alphabetic*/
bool is_alnum(char c);/*check if a character is alphanumeric*/
bool is_digit(char c);/*check if a character is a digit*/
bool is_valid_operand(const char *operand);/*check if an operand is valid*/
addressing_type get_addressing_type(char *operand);/*get the addressing type for an operand*/
opcode get_opcode(const char *operation);/*get the opcode corresponding to an operation*/


#endif /* UTILS_H */

