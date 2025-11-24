//
// dump a shape file for debug
//

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "shape.h"

#define DEBUG

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

char buff[80];

int main( int argc, char *argv[]) {

  f_shape shape;
  coord_t coord;
  int num = 0;
  int count;

  FILE *fp;
  FILE *fv;

  int plot_mode = 0;

  if( argc < 2) {
    fprintf( stderr, "usage: %s [-p] shapeset\n", argv[0]);
    return 1;
  }

  for( int i=1; i<argc; i++) {
    if( *argv[i] == '-') {
      switch( toupper( argv[i][1])) {
      case 'P':
	plot_mode = 1;
	break;
      default:
	fprintf( stderr, "unknown option %s\n", argv[i]);
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
    }
  }
  
  fread( &count, sizeof(int), 1, fp);

  for( num=0; num<count; num++) {
    fread( &shape, sizeof(shape), 1, fp);

    //    long cpos = ftell( fp);
    if( !plot_mode) {
      printf("SHAPE %d: ", num);
      print_fshape( &shape);
    }

    //    fseek( fv, (long)shape.lat, SEEK_SET);
    if( plot_mode) {
      printf("%d\n", shape.nvert);
      coord_t* lats = calloc( sizeof(coord_t), shape.nvert);
      coord_t* lons = calloc( sizeof(coord_t), shape.nvert);
      for( int i=0; i<shape.nvert; i++)
	fread( &lats[i], sizeof(coord_t), 1, fv);
      for( int i=0; i<shape.nvert; i++)
	fread( &lons[i], sizeof(coord_t), 1, fv);
      for( int i=0; i<shape.nvert; i++)
	printf("%f %f\n", lats[i], lons[i]);
      free( lats);
      free( lons);
    } else {
      printf("  lat: ");
      for( int i=0; i<shape.nvert; i++) {
	fread( &coord, sizeof(coord_t), 1, fv);
	if( i < 5)
	  printf(" %f", coord);
      }
      printf("\n");
      //    fseek( fv, (long)shape.lon, SEEK_SET);
      printf("  lon: ");
      for( int i=0; i<shape.nvert; i++) {
	fread( &coord, sizeof(coord_t), 1, fv);
	if( i < 5)
	  printf(" %f", coord);
      }
      printf("\n");
    } // if( plot_mode) else

  } // for( num...)

  return 0;
}
