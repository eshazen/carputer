//
// dump a shape file for debug
//

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// #define DEBUG

#include "shape.h"

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

char buff[80];
int verbose = 0;

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
  int num = 0;
  int32_t count;

  FILE *fp;
  FILE *fv;
  FILE *fa;

  uint32_t poff;

  int plot_mode = 0;
  FILE *fplot;

  char *matches[MAXMATCH];
  int nmatch = 0;

  int part_first = 0;
  int part_last = 99999;

  a_point pt;

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
      case 'P':
	if( i == argc-1) {
	  fprintf( stderr, "Missing plot file after -P\n");
	  return 1;
	}
	++i;
	plot_mode = 1;
	fplot = fopen( argv[i], "w");
	break;
      case 'A':
	if( i == argc-2) {
	  fprintf( stderr, "Missing part range after -a\n");
	  return 1;
	}
	++i;
	part_first = atoi( argv[i]);
	++i;
	part_last = atoi( argv[i]);
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
  

  // pass 1:
  if( verbose) {
    printf( "--- PASS 1 ---\n");
    fread( &count, sizeof(count), 1, fp);
    printf( "** Count: %d\n", count);
    for( num=0; num<count; num++) {
      fread( &fshape, sizeof(fshape), 1, fp);
      printf( "SHAPE: %d\n", num);
      print_fshape( &fshape);
    }
    printf( "--- PASS 2 ---\n");
  }
    
  rewind( fp);

  fread( &count, sizeof(count), 1, fp);
  printf( "Count: %d\n", count);

  for( num=0; num<count; num++) {
    fread( &fshape, sizeof(fshape), 1, fp);

//    if( nmatch && !find_match( fshape.name, matches, nmatch))
//      continue;

    if( verbose) {
      printf( "\nFSHAPE_NO %d:\n", num);
      printf( "nvert: %d  nparts: %d  (%f..%f) (%f..%f)\n",
	       fshape.nvert, fshape.nparts,
	       fshape.minLat, fshape.maxLat, fshape.minLon, fshape.maxLon);
      printf( "points_off: %d  parts_off: %d\n", fshape.points_off, fshape.part_off);
    }

    if( verbose)
      print_fshape( &fshape);

    if( fshape.nvert > 500000 || fshape.nparts > 5000 || fshape.nvert < 1) {
      fprintf( stderr, "Values seem unreasonable!\n");
      exit(1);
    }


    fseek( fv, fshape.points_off, SEEK_SET);
    for( int i=0; i<fshape.nvert; i++) {
      fread( &pt, sizeof(pt), 1, fv);
      if( verbose > 1) {
	printf( "  %d @ %d: (%f,%f)\n", i, fshape.points_off+i, pt.lat, pt.lon);
      } else {
	if( i < 5 && verbose)
	  printf( " (%f,%f)", pt.lat, pt.lon);
      }
    }
    if( verbose)
      printf( "\n");

    int p_start, p_end;
       
    // two passes through parts, first to display
    if( verbose)
      printf( "\nPARTS (%d):\n", fshape.nparts);
    if( fseek( fa, fshape.part_off, SEEK_SET)) {
      fprintf( stderr, "Seek error to %d on parts file\n", fshape.part_off);
      exit(1);
    }
    for( int i=0; i<fshape.nparts; i++) {
      fread( &poff, sizeof(poff), 1, fa);
      p_start = poff;
      if( verbose)
	printf(  " %d", poff);
#ifdef DEBUG
      fprintf( stderr, "CHECK i=%d nparts=%d nvert=%d\n", i, fshape.nparts, fshape.nvert);
#endif
      if( i == fshape.nparts-1) {
	p_end = fshape.nvert;
#ifdef DEBUG
	fprintf( stderr, "SET START = %d END = %d\n", p_start, p_end);
#endif
      }
      else {
	// silly dance to read ahead one and put it back
	uint32_t toff;
	long tpos = ftell( fa);
	fread( &toff, sizeof(toff), 1, fa);
	p_end = toff;
	fseek( fa, tpos, SEEK_SET);
      }
      if( verbose)
	printf( " %d [%d-%d] (%d)\n", i, p_start, p_end, p_end-p_start);
    }

    // back to start of list
    fseek( fa, fshape.part_off, SEEK_SET);
    for( int i=0; i<fshape.nparts; i++) {
      fread( &poff, sizeof(poff), 1, fa);
      p_start = poff;
      if( verbose)
	printf(  " %d", poff);
      if( i == fshape.nparts-1)
	p_end = fshape.nvert;
      else {
	// silly dance to read ahead one and put it back
	uint32_t toff;
	long tpos = ftell( fa);
	fread( &toff, sizeof(toff), 1, fa);
	p_end = toff;
	fseek( fa, tpos, SEEK_SET);
      }
      if( i >= part_first && i <= part_last) {
	if( verbose)
	  printf( "*");
	// now we have the virtex range in p_start, p_end
	fseek( fv, p_start*sizeof(a_point)+fshape.points_off, SEEK_SET);
	if( verbose > 1)
	  printf( "\n part len = %d (%d to %d)\n", p_end-p_start, p_start, p_end);
	if( p_end == p_start) {
	  fprintf( stderr, "ERROR:  p_end == p_start = %d\n", p_end);
	  fprintf( stderr, "part %d with part_off %d\n", i, fshape.part_off);
	  exit(1);
	}
	if( plot_mode)
	  fprintf( fplot, "%d\n", (p_end-p_start));
	for( int v=p_start; v<p_end; v++) {
	  fread( &pt, sizeof(pt), 1, fv);
	  if( plot_mode)
	    fprintf( fplot, "%f %f\n", pt.lon, pt.lat);
	  else
	    if( verbose > 1)
	      printf( "  %d: (%f, %f)\n", v, pt.lat, pt.lon);
	}
      } 
    }
    if( verbose)
      printf( "\n");

  } // for( num...)

  return 0;
}
