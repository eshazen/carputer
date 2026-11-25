//
// lookup a lat/long using R-Tree and location from files
//

#include <stdio.h>
#include <ctype.h>

#include "filetree.h"
#include "inside.c"

static char buff[80];

void search_tree( coord_t lat, coord_t lon, long offset, FILE *ft, FILE *fd, FILE *fv, FILE *fa);

int main( int argc, char *argv[]) {

  a_fileset fs;
  coord_t lat, lon;

  if( argc < 4) {
    fprintf( stderr, "usage: %s fileset -L lat lon\n", argv[0]);
    return 1;
  }

  for( int i=1; i<argc; i++) {
    if( *argv[i] == '-') {
      switch( toupper( argv[i][1])) {
      case 'L':
	if( i > argc-2) {
	  fprintf( stderr, "Missing lat lon after -L\n");
	  return 1;
	}
	++i;
	lat = atof( argv[i]);
	++i;
	lon = atof( argv[i]);
	break;
      default:
	fprintf( stderr, "Unknown option %s\n", argv[i]);
	break;
      }
    } else {
      if( open_set( argv[i], &fs, "rb")) {
	fprintf( stderr, "Error opening fileset %s\n", argv[i]);
	return 1;
      }
    }
  }

  search_tree( lat, lon, 0L, fs.ree, fs.dat, fs.vrt, fs.prt);		/* start at root */
}

// #define MSIZ 0.05

void search_tree( coord_t lat, coord_t lon, long offset, FILE *ft, FILE *fd, FILE *fv, FILE *fa) {

  struct f_node fnode;
  int rc;
  f_shape fshape;

  fseek( ft, offset, SEEK_SET);
  rc = fread( &fnode, sizeof(fnode), 1, ft);
  if( rc != 1) {
    printf("Read error %d on tree at offset %ld\n", rc, offset);
    exit( 1);
  }

  // check against rects
  for( int i=0; i<fnode.count; i++) {
    if( lat >= fnode.rects[i].min[0] && lat <= fnode.rects[i].max[0] &&
	lon >= fnode.rects[i].min[1] && lon <= fnode.rects[i].max[1]) {
      if( fnode.kind == LEAF) { /* LEAF */
	fseek( fd, fnode.item_offsets[i], SEEK_SET);
	fread( &fshape, sizeof(fshape), 1, fd);
	// now point to the virtex list
	fseek( fv, fshape.points_off, SEEK_SET);

	// loop over parts
	if( fseek( fa, fshape.part_off, SEEK_SET)) {
	  fprintf( stderr, "Error seeking to offset %d in PRT file\n", fshape.part_off);
	}
	  
	for( int j=0; j<fshape.nparts; j++) {
	  a_part prt;
	  fread( &prt, sizeof(prt), 1, fa);

	  long so = prt.offset*sizeof(a_point)+fshape.points_off;
	  if( fseek( fv, so, SEEK_SET)) {
	    fprintf( stderr, "Seek error to %ld in VRT\n", so);
	    exit(1);
	  }

	  // version with no heap
	  if( point_in_part_file( fv, prt.count, lon, lat ))
	    printf("POINT %f %f is INSIDE %s prio %d\n", lat, lon, fshape.name, fshape.prio);
	}
	  
      } else {		  /* BRANCH */
	search_tree( lat, lon, fnode.node_offsets[i]*sizeof(fnode), ft, fd, fv, fa);
      }
    }
  }
}
