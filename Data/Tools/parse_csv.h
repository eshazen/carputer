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
int parse_csv_line(char *line, char *fields[], int max_fields) {
    int count = 0;
    char *p = line;

    while (*p && count < max_fields) {
        // Skip leading whitespace (optional)
        while (isspace((unsigned char)*p)) p++;

        if (*p == '"') {
            // Quoted field
            p++; // skip opening quote
            fields[count++] = p;

            // Parse until closing quote
            while (*p) {
                if (*p == '"' && *(p + 1) == '"') {
                    // Escaped quote ""
                    memmove(p, p + 1, strlen(p)); // remove one quote
                    p++;
                } else if (*p == '"') {
                    // End of quoted field
                    *p = '\0';
                    p++;
                    // Skip until comma or end
                    while (*p && *p != ',') p++;
                    if (*p == ',') p++;
                    break;
                } else {
                    p++;
                }
            }
        } else {
            // Unquoted field
            fields[count++] = p;
            while (*p && *p != ',') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
        }
    }
    return count;
}

