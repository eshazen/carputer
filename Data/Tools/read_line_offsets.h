// ChatGPT query:
//
// Please write a C function to read an ASCII text file and build an
// index of the starting offset of each line in the file
//

#include <stdio.h>
#include <stdlib.h>

/*
 * read_line_offsets:
 *   Reads an ASCII text file and builds an array of offsets marking
 *   the start of each line.
 *
 * Parameters:
 *   filename     - path to the file
 *   offsets_out - pointer to array of long (returned, must be freed by caller)
 *
 * Returns:
 *   Number of lines in file on success, or -1 on error.
 */
long read_line_offsets(const char *filename, long **offsets_out)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    // Find file size
    if (fseek(fp, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(fp);
        return -1;
    }
    long file_size = ftell(fp);
    rewind(fp);

    // Allocate initial offsets array
    size_t capacity = 1024;   // start with space for 1024 lines
    long *offsets = malloc(capacity * sizeof(long));
    if (!offsets) {
        perror("malloc");
        fclose(fp);
        return -1;
    }

    long pos = 0; // current offset
    int c;
    size_t count = 0;

    // First line always starts at offset 0
    offsets[count++] = 0;

    while ((c = fgetc(fp)) != EOF) {
        pos++;
        if (c == '\n' && pos < file_size) {
            // Start of next line = current position
            if (count == capacity) {
                capacity *= 2;
                long *tmp = realloc(offsets, capacity * sizeof(long));
                if (!tmp) {
                    perror("realloc");
                    free(offsets);
                    fclose(fp);
                    return -1;
                }
                offsets = tmp;
            }
            offsets[count++] = pos;
        }
    }

    fclose(fp);

    *offsets_out = offsets;
    return (long)count;
}

/* Example usage:
#include <stdio.h>
int main(void) {
    long *offsets;
    long nlines = read_line_offsets("test.txt", &offsets);
    if (nlines < 0) return 1;
    for (long i = 0; i < nlines; i++) {
        printf("Line %ld starts at offset %ld\n", i+1, offsets[i]);
    }
    free(offsets);
    return 0;
}
*/
