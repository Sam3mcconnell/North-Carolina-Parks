/**
    @file input.c
    @author Samuel E McConnell (semcconn)
    This file has one function. This file is used to read a line from the file and return it.
    This file is used by other functions to read the files needed to make the prgram run.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

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
char *readLine(FILE *fp)
{
    int size = INITIAL_BUFFEER_SIZE;
    char *line = (char *)malloc(size * sizeof(line));

    if (line == NULL)
    {
        fprintf(stderr, "some error");
        exit(EXIT_FAILURE);
    }

    int position = 0;
    char c;

    while (1)
    {
        c = fgetc(fp);

        if (c == EOF || c == '\n')
        {
            line[position] = '\0';
            break;
        }
        else
        {
            line[position] = (char)c;
        }
        position++;
        if (position >= size)
        {
            size *= 2;
            line = (char *)realloc(line, size * sizeof(char));
        }
    }
    if (position == 0 && c == EOF)
    {
        free(line);
        return NULL;
    }

    return line;
}