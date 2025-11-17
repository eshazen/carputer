//
// dump a shape file for debug
//

#include <stdio.h>
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

  snprintf( buff, sizeof(buff), "%s.DAT", argv[1]);
  if( (fp = fopen( buff, "rb")) == NULL) {
    fprintf( stderr, "Can't open %s for input\n", buff);
    return 1;
  }
  snprintf( buff, sizeof(buff), "%s.VRT", argv[1]);
  if( (fv = fopen( buff, "rb")) == NULL) {
    fprintf( stderr, "Can't open %s for input\n", buff);
    return 1;
  }
  
  fread( &count, sizeof(int), 1, fp);

  for( num=0; num<count; num++) {
    fread( &shape, sizeof(shape), 1, fp);

    //    long cpos = ftell( fp);
    printf("SHAPE %d: ", num);
    print_fshape( &shape);

    //    fseek( fv, (long)shape.lat, SEEK_SET);
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
  }

  return 0;
}
