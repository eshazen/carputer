//
// dump a shape file for debug
//

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "shape.h"

#define DEBUG

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

char buff[80];

#define MAXMATCH 10

// search in string name, look for occurrences of strings in match[0..nmatch-1]
// return index of first string which matches + 1, or 0 if no match
//
int find_match( char *name, char* matches[], int nmatch) {
  for( int i=0; i<nmatch; i++) {
    if( strcasestr( name, matches[i])) {
      return i+1;
    }
  }
  return 0;
}


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

  char *matches[MAXMATCH];
  int nmatch = 0;

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
      case 'M':
	if( i == argc-1) {
	  fprintf( stderr, "Missing match string after -M\n");
	  return 1;
	}
	++i;

	// split argv[i] on "|" into match strings
	matches[nmatch] = strtok( argv[i], "|");
	++nmatch;
	while( (matches[nmatch] = strtok( NULL, "|")) && nmatch < MAXMATCH)
	  ++nmatch;
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

    if( !find_match( fshape.name, matches, nmatch))
      continue;

    fprintf( stderr, "\nFSHAPE %d:\n", num);
    fprintf( stderr, "nvert: %d  nparts: %d  (%f..%f) (%f..%f)\n",
	   fshape.nvert, fshape.nparts,
	   fshape.minLat, fshape.maxLat, fshape.minLon, fshape.maxLon);
    fprintf( stderr, "points_off: %d  parts_off: %d\n", fshape.points_off, fshape.part_off);

    if( fshape.nvert > 100000 || fshape.nparts > 1000) {
      fprintf( stderr, "Values seem unreasonable!\n");
      exit(1);
    }

    print_fshape( &fshape);

    fseek( fv, fshape.points_off, SEEK_SET);
    for( int i=0; i<fshape.nvert; i++) {
      fread( &pt, sizeof(pt), 1, fv);
      if( i < 5)
	fprintf( stderr, " (%f,%f)", pt.lat, pt.lon);
    }
    fprintf( stderr, "\n");
       
    fprintf( stderr, "\nPARTS:\n");
    if( fseek( fa, fshape.part_off, SEEK_SET)) {
      fprintf( stderr, "Seek error to %d on parts file\n", fshape.part_off);
      exit(1);
    }

    for( int i=0; i<fshape.nparts; i++) {
      fread( &poff, sizeof(poff), 1, fa);
      fprintf( stderr,  " %d", poff);
    }
    fprintf( stderr, "\n");

  } // for( num...)

  return 0;
}
