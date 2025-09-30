// ChatGPT Query:
//
// "please write a c function to parse a CSV string.  return an array
// of pointers to the start of each item.  Please correctly handle
// quoted strings by discarding the quotes."

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * parse_csv_line()
 *   Parses a CSV line into fields.
 *   - line: input CSV string (modified in place)
 *   - fields: output array of char* pointers to each field
 *   - max_fields: size of fields array
 * Returns: number of fields found
 *
 * Note: The function modifies the input line (adds '\0' terminators).
 *
 */
int parse_csv_line(char *line, char *fields[], int max_fields);
