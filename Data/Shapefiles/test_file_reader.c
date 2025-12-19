
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "shape.h"

static char buff[80];

void read_line( char*s, FILE *fp) {
  do { 
    fgets( buff, sizeof(buff), fp); 
    printf("read_line() = %s\n", buff);
  } while( *buff == '#');
}

//
// read shapes from a simple text file
// allocate memory from heap and return
// very little error checking
//
a_shape* read_test_file( FILE*fp, int *num) {

  a_shape* s;
  int nshape = 0;

  coord_t minLat, minLon, maxLat, maxLon;

  s = calloc( 1, sizeof(a_shape));

  read_line( buff, fp);
  printf("Read: %s\n", buff);

  if( !strncasecmp( buff, "COUNT", 5)) {
    sscanf( buff, "%*s %d", &nshape);
    s = calloc( nshape, sizeof(a_shape));
    printf("Allocated space for %d shapes\n", nshape);
  }

  *num = nshape;

  for( int sno=0; sno<nshape; sno++) {
    printf("Reading shape %d\n", sno);

    read_line( buff, fp);
    if( !strncasecmp( buff, "SHAPE", 5)) {

      sscanf( buff, "%*s %d", &s[sno].nvert);

      printf("Shape %d has %d verts\n", sno, s[sno].nvert);
      s[sno].lat = calloc( s[sno].nvert, sizeof(coord_t));
      s[sno].lon = calloc( s[sno].nvert, sizeof(coord_t));
    
      read_line( buff, fp);
      strcpy( s[sno].name, buff);
      printf("name = %s\n", s[sno].name);
      for( int i=0; i<s[sno].nvert; i++) {
	read_line( buff, fp);
	sscanf( buff, "%f %f", &s[sno].lat[i], &s[sno].lon[i]);
	printf("   VERT[%d]: %f %f\n", i, s[sno].lat[i], s[sno].lon[i]);
      }
      // calculate min/max
      minLat = 999;
      minLon = 999;
      maxLat = -999;
      maxLon = -999;
      for( int i=0; i<s[sno].nvert; i++) {
	if( s[sno].lat[i] < minLat)
	  minLat = s[sno].lat[i];
	if( s[sno].lon[i] < minLon)
	  minLon = s[sno].lon[i];
	if( s[sno].lat[i] > maxLat)
	  maxLat = s[sno].lat[i];
	if( s[sno].lon[i] > maxLon)
	  maxLon = s[sno].lon[i];
      }
      s[sno].minLat = minLat;
      s[sno].minLon = minLon;
      s[sno].maxLat = maxLat;
      s[sno].maxLon = maxLon;
    } else {
      printf("Missing SHAPE, saw %s\n", buff);
      exit(1);
    }

    
  }
  return s;
}


