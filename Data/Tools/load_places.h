//
// read places file
// load into dynamically-allocated array of structs
// return a pointer to the array of structs
// set nplaces to the count
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_csv.h"

#include "places.h"

static char buffer[256];
static char* toks[10];

a_place* load_places( char *file, int* nplaces) {

  static a_place* ap;

  FILE *fp = fopen( file, "rb");
  if( fp == NULL) {
    printf("Can't open %s\n", file);
    return NULL;
  }

  // first pass to count records
  int nrec = 0;
  while( fgets( buffer, sizeof(buffer), fp))
    ++nrec;
  rewind( fp);

  ap = calloc( nrec, sizeof(a_place));
  if( ap == NULL) {
    fprintf( stderr, "Malloc failed (%ld bytes)\n", sizeof(a_place)*nrec);
    return NULL;
  }
  
  int n = 0;
  while( fgets( buffer, sizeof(buffer), fp)) {

    if( csv_to_place( &ap[n], buffer)) {
      fprintf( stderr, "Error in line: %s\n", buffer);
      return NULL;
    }
    ++n;
  }
  *nplaces = n;
  return ap;
}
