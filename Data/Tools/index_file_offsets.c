#include <stdio.h>

#include "read_line_offsets.h"

int main( int argc, char *argv[]) {
  long *offsets;
  long nlines = read_line_offsets( argv[1], &offsets);

  fprintf( stderr, "Read %ld lines\n", nlines);

  // output a list of the offsets
  for( long i=0; i<nlines; i++)
    printf("%ld\n", offsets[i]);
}

