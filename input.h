/**
    @file input.h
    @author Samuel E McConnell (semcconn)
    This is the helper file for input.c. This file lets the program use the function from
    input.c.
*/

/** This is the initial buffer size for the line */
#define INITIAL_BUFFEER_SIZE 50

/**
    This function reads a single line of input from the given input stream (stdin or a file) and returns
    it as a string inside a block of dynamically allocated memory. You can use this function to read
    commands from the user and to read park descriptions from a park file. Inside the function, you should
    implement a resizable array to read in a line of text that could be arbitrarily large. If there’s no more
    input to read, this function should return NULL. Since this function returns a pointer to dynamically
    allocated memory, some other code will be responsible for eventually freeing that memory (to avoid a
    memory leak).
    @param fp as the pointer to a file to read
    @return a pointer to a char string as the line it read from the file
*/
char *readLine(FILE *fp);
