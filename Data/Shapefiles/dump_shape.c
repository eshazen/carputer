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

  f_shape fshape;
  coord_t coord;
  int num = 0;
  int count;

  FILE *fp;
  FILE *fv;
  FILE *fa;

  uint32_t poff;

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
      snprintf( buff, sizeof(buff), "%s.PRT", argv[i]);
      if( (fa = fopen( buff, "rb")) == NULL) {
	fprintf( stderr, "Can't open %s for input\n", buff);
	return 1;
      }
    }
  }
  
  fread( &count, sizeof(int), 1, fp);

  fprintf( stderr, "Count: %d\n", count);

  a_point pt;

  for( num=0; num<count; num++) {
    fread( &fshape, sizeof(fshape), 1, fp);

    if( !plot_mode) {
      fprintf( stderr, "\nFSHAPE %d:\n", num);
      print_fshape( &fshape);

      printf("nvert: %d  nparts: %d  (%f..%f) (%f..%f)\n",
	     fshape.nvert, fshape.nparts,
	     fshape.minLat, fshape.maxLat, fshape.minLon, fshape.maxLon);
      printf("points_off: %d  parts_off: %d\n", fshape.points_off, fshape.part_off);
      fseek( fv, fshape.points_off, SEEK_SET);
      for( int i=0; i<fshape.nvert; i++) {
	fread( &pt, sizeof(pt), 1, fv);
	if( i < 5)
	  printf(" (%f,%f)", pt.lat, pt.lon);
      }
      printf("\nPARTS:\n");
      fseek( fa, fshape.part_off, SEEK_SET);
      for( int i=0; i<fshape.nparts; i++) {
	fread( &poff, sizeof(poff), 1, fp);
	printf( " %d", poff);
      }
      printf("\n");

    if( num > 3) exit(1);

//    printf("  %d parts\n", fshape.nparts);
//    fseek( fa, fshape.part_off, SEEK_SET);
//    for( int k=0; k<fshape.nparts; k++) {
//      fread( &poff, sizeof(poff), 1, fa);
//      printf("  %d %d\n", k, poff);
//    }
//    
//    a_point* points = calloc( sizeof(a_point), fshape.nvert);
//    for( int i=0; i<fshape.nvert; i++) {
//      fread( &points[i].lat, sizeof(coord_t), 1, fv);
//      fread( &points[i].lon, sizeof(coord_t), 1, fv);
//    }
//
//    if( plot_mode) {
//      printf("%d\n", fshape.nvert);
//      for( int i=0; i<fshape.nvert; i++)
//	printf("%f %f\n", points[i].lat, points[i].lon);
//    } else {
//      printf("  lat: ");
//      for( int i=0; i<fshape.nvert; i++) {
//	if( i < 5)
//	  printf(" %f", points[i].lat);
//      }
//      printf("...\n");
//      printf("  lon: ");
//      for( int i=0; i<fshape.nvert; i++) {
//	if( i < 5)
//	  printf(" %f", points[i].lon);
//      }
//      printf("...\n");

    } // if( plot_mode) else

//    free( points);
    
  } // for( num...)

  return 0;
}
