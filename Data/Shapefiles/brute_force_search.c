//
// brute force search for lat/long within rectangle
//

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "shape.h"

char buff[80];
int verbose = 0;

int main( int argc, char *argv[]) {

  f_shape fshape;
  int num = 0;
  int32_t count;

  a_point pt;

  coord_t lat, lon;

  FILE *fp;
  FILE *fv;
  FILE *fa;

  if( argc < 2) {
    fprintf( stderr, "usage: %s shapeset [-m match_string][-p] [-a n m]\n", argv[0]);
    return 1;
  }

  for( int i=1; i<argc; i++) {
    if( *argv[i] == '-') {
      switch( toupper( argv[i][1])) {
      case 'V':
	verbose++;
	break;
	fprintf( stderr, "unknown option %s\n", argv[i]);
	break;
      case 'L':
	if( i > argc-1) {
	  fprintf( stderr, "Need lat lon after -L\n");
	  return 1;
	}
	++i;
	lat = atof( argv[i]);
	++i;
	lon = atof( argv[i]);
	break;
      }
    } else {
      snprintf( buff, sizeof(buff), "%s.DAT", argv[i]);
      if( (fp = fopen( buff, "rb")) == NULL) {
	fprintf( stderr, "Can't open %s for input\n", buff);
	return 1;
      }
      snprintf( buff, sizeof(buff), "%s.VRT", argv[i]);
      if( (fv = fopen( buff, "rb")) == NULL) {
	fprintf( stderr, "Can't open %s for input\n", buff);
	return 1;
      }
      snprintf( buff, sizeof(buff), "%s.PRT", argv[i]);
      if( (fa = fopen( buff, "rb")) == NULL) {
	fprintf( stderr, "Can't open %s for input\n", buff);
	return 1;
      }
    }
  }
  

  fread( &count, sizeof(count), 1, fp);
  printf( "Count: %d\n", count);

  for( num=0; num<count; num++) {
    fread( &fshape, sizeof(fshape), 1, fp);

    if( lat >= fshape.minLat && lat <= fshape.maxLat && 
	lon >= fshape.minLon && lon <= fshape.maxLon)
      print_fshape( &fshape);

  } // for( num...)

  return 0;
}
