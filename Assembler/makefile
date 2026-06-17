#compiler
CC = gcc

#compiler flags
CFLAGS = -g -ansi -pedantic -Wall

#executable name
TARGET = assembler

#source files
SRCS = preprocessor.c first_pass.c second_pass.c utils.c file_writer.c assemble.c

#object files
OBJS = $(SRCS:.c=.o)

#header files
HDRS = preprocessor.h first_pass.h second_pass.h utils.h file_writer.h definitions.h

#default rule (build the executable)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

#rule to build object files from source files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

#clean rule (remove object files and the executable)
clean:
	rm -f $(OBJS) $(TARGET)

#PHONY target to prevent conflict with files named 'clean'
.PHONY: clean

