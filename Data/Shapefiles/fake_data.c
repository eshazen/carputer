//
// Write .DAT and .VRT files with test data
// 

// #define VERBOSE

// skip the ray-intersect algorithm
// #define RANGE_ONLY

#include "shape.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "shapefil.h"
#include "test_file_reader.h"

char buff[80];

int main(int argc, char **argv) {

  FILE *fo = NULL;
  FILE *fv = NULL;
  FILE *ft = NULL;

  int fna = -1;
  a_shape *shapes;

  int nEntities;

  if (argc < 2) {
    fprintf( stderr, "Usage: %s [-o output]\n", argv[0]);
    return 1;
  }

  for( int i=1; i<argc; i++) {
    printf("Looking at %s\n", argv[i]);
    if( *argv[i] == '-') {
      switch( toupper( argv[i][1])) {
      case 'O':
	if( i == argc-1) {
	  fprintf( stderr, "Missing filename after -O\n");
	  return 1;
	}
	++i;
	snprintf( buff, sizeof( buff), "%s.DAT", argv[i]);
	if( (fo = fopen( buff, "wb")) == NULL) {
	  fprintf( stderr, "Error opening output file %s\n", buff);
	  return 1;
	}
	snprintf( buff, sizeof( buff), "%s.VRT", argv[i]);
	if( (fv = fopen( buff, "wb")) == NULL) {
	  fprintf( stderr, "Error opening output file %s\n", buff);
	  return 1;
	}
	snprintf( buff, sizeof( buff), "%s.TEST", argv[i]);
	if( (ft = fopen( buff, "rb")) == NULL) {
	  fprintf( stderr, "Error opening input file %s\n", buff);
	  return 1;
	}
	break;
      default:
	fprintf( stderr, "Unknown option: %s\n", argv[i]);
	return 1;
      }
    } else {
      nEntities = atoi( argv[i]);
    }
    
  }

  // read the test file
  printf("--- read test file ---\n");
  shapes = read_test_file( ft, &nEntities);

  printf("--- print shapes ---\n");
  for (int i = 0; i < nEntities; i++) {
    printf("Record %d:\n", i);
    print_shape( &shapes[i]);
  }

  printf("--- write shapes ---\n");
  if( fo) {
    write_shapes( shapes, nEntities, fo, fv);
  }

  return 0;
}
