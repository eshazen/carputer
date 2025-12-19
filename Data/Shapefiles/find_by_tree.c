//
// lookup a lat/long using R-Tree and location from files
//

#include <stdio.h>
#include <ctype.h>

// polygon load/search
#define LOAD_POLY

#include "filetree.h"
#include "inside.c"

static char buff[80];

int verbose = 0;

int plot_mode = 0;
FILE *fplot;

void search_tree( coord_t lat, coord_t lon, long offset, FILE *ft, FILE *fd, FILE *fv, FILE *fa);

int main( int argc, char *argv[]) {

  a_fileset fs;
  coord_t lat, lon;

  if( argc < 4) {
    fprintf( stderr, "usage: %s fileset lat lon\n", argv[0]);
    return 1;
  }

  for( int i=1; i<argc; i++) {
    if( *argv[i] == '-') {
      switch( toupper( argv[i][1])) {
      case 'P':
	if( i == argc-1) {
	  fprintf( stderr, "Missing plot file after -P\n");
	  return 1;
	}
	++i;
	plot_mode = 1;
	fplot = fopen( argv[i], "w");
	break;
      case 'V':
	++verbose;
	break;
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

#define MSIZ 0.05

void search_tree( coord_t lat, coord_t lon, long offset, FILE *ft, FILE *fd, FILE *fv, FILE *fa) {

  struct f_node fnode;
  int rc;
  f_shape fshape;

  if( plot_mode) {
    fprintf( fplot, "5\n");
    fprintf( fplot, "%f %f\n", lon-MSIZ, lat);
    fprintf( fplot, "%f %f\n", lon, lat+MSIZ);
    fprintf( fplot, "%f %f\n", lon+MSIZ, lat);
    fprintf( fplot, "%f %f\n", lon, lat-MSIZ);
    fprintf( fplot, "%f %f\n", lon-MSIZ, lat);
  }


  fseek( ft, offset, SEEK_SET);
  rc = fread( &fnode, sizeof(fnode), 1, ft);
  if( rc != 1) {
    printf("Read error %d on tree at offset %ld\n", rc, offset);
    exit( 1);
  }

  if( verbose)
    printf("NODE: %ld type=%d count=%d\n", offset, fnode.kind, fnode.count);
    // check against rects
    for( int i=0; i<fnode.count; i++) {
      if( verbose)
	printf("Compare lat %f vs (%f..%f) and lon %f vs (%f..%f)\n",
	       lat, fnode.rects[i].min[0], fnode.rects[i].max[0],
	       lon, fnode.rects[i].min[1], fnode.rects[i].max[1]);
      if( lat >= fnode.rects[i].min[0] && lat <= fnode.rects[i].max[0] &&
	  lon >= fnode.rects[i].min[1] && lon <= fnode.rects[i].max[1]) {
	if( verbose)
	  printf("MATCH at index %d... ", i);
	if( fnode.kind == LEAF) { /* LEAF */
	  if( verbose)
	    printf("LEAF: %d...", fnode.item_offsets[i]);
	  fseek( fd, fnode.item_offsets[i], SEEK_SET);
	  fread( &fshape, sizeof(fshape), 1, fd);
	  // now point to the virtex list
	  if( verbose)
	    printf("Reading %d vertexes\n", fshape.nvert);
	  fseek( fv, fshape.points_off, SEEK_SET);
#ifdef LOAD_POLY
	  a_point* point_ptr = calloc( fshape.nvert, sizeof(a_point));
	  fread( point_ptr, sizeof(a_point), fshape.nvert, fv);
	  if( verbose) {
	    for( int k=0; k<fshape.nvert; k++) {
	      if( verbose) 
		printf("(%f,%f) ", point_ptr[k].lat, point_ptr[k].lon);
	      if( (k % 10) == 0)
		if( verbose)
		  printf("\n");
	    }
	    if( verbose)
	      printf("\n");
	  }
#endif
	  if( verbose) {
	    printf("LEAF SHAPE:\n");
	    print_fshape( &fshape);
	  }

#ifdef LOAD_POLY
	  // loop over parts
	  if( verbose)
	    printf("seek part to %d\n", fshape.part_off);
	  if( fseek( fa, fshape.part_off, SEEK_SET)) {
	    fprintf( stderr, "Error seeking to offset %d in PRT file\n", fshape.part_off);
	  }

#ifdef NEW_PART
	  
	  for( int j=0; j<fshape.nparts; j++) {
	    a_part prt;
	    fread( &prt, sizeof(prt), 1, fa);
	    if( verbose)
	      printf("Part %d count: %d offset: %d\n", j, prt.count, prt.offset);

	    long so = prt.offset*sizeof(a_point)+fshape.points_off;
	    if( verbose)
	      printf("Seeking to %ld in VRT\n", so);
	    if( fseek( fv, so, SEEK_SET)) {
	      fprintf( stderr, "Seek error to %ld in VRT\n", so);
	      exit(1);
	    }

	    a_point* pointz = calloc( prt.count, sizeof( a_point));
	    if( !pointz) {
	      fprintf( stderr, "malloc failed for %ld bytes\n", prt.count * sizeof(a_point));
	      exit(1);
	    }

	    int rc = fread( pointz, sizeof(a_point), prt.count, fv);
	    if( rc != prt.count) {
	      fprintf( stderr, "Error reading %d points got %d\n", prt.count, rc);
	      exit(1);
	    }

	    if( plot_mode)
	      fprintf( fplot, "%d\n", prt.count);
	    for( int k=0; k<prt.count; k++) {
	      if( verbose)
		printf("  %d: %f %f\n", k, pointz[k].lat, pointz[k].lon);
	      if( plot_mode)
		fprintf( fplot, "%f %f\n", pointz[k].lon, pointz[k].lat);
	    }

	    if( point_in_part( pointz, prt.count, lon, lat))
	      printf("POINT %f %f is INSIDE %s prio %d\n", lat, lon, fshape.name, fshape.prio);
	    else
	      if( verbose)
		printf("POINT %f %f is OUTSIDE %s prio %d\n", lat, lon, fshape.name, fshape.prio);
	    free( pointz);
	  }
	  

#else	  
	  uint32_t p_start, p_end;
	  if( verbose)
	    printf("%d parts\n", fshape.nparts);
	  for( int j=0; j<fshape.nparts; j++) {
	    fread( &p_start, sizeof(p_start), 1, fa); /* read offset for start */
	    if( j == fshape.nparts-1) /* end of list? */
	      p_end = fshape.nvert;
	    else {
	      // read ahead one
	      long pos = ftell( fa);
	      fread( &p_end, sizeof(p_end), 1, fa); /* read offset for end */
	      fseek( fa, pos, SEEK_SET);
	    }
	    if( verbose)
	      printf( "Part %d from %d to %d\n", j, p_start, p_end);
	    int psiz = p_end-p_start;
	    if( psiz < 1 || psiz > 10000) {
	      fprintf( stderr, "Bad part size %d\n", psiz);
	      exit(1);
	    }
	    a_point* ppart = calloc( psiz, sizeof( a_point));

	    for( int p=0; p<psiz; p++)
	      fread( &ppart[p], sizeof(a_point), 1, fa);

	    if( point_in_part( ppart, psiz, lat, lon))
	      printf("is INSIDE!\n");
	    else
	      printf("outside\n");
	    free( ppart);
	  }
#endif	  
#endif
	  
//	  if( point_in_polygon( lat_ptr, lon_ptr, fshape.nvert, lat, lon))
//	    printf("is INSIDE!\n");
//	  else
//	    printf("outside\n");


#ifdef LOAD_POLY
	  free( point_ptr);
#endif	  
	} else {		  /* BRANCH */
	  if( verbose)
	    printf("BRANCH: go down to node %d offset %ld\n", fnode.node_offsets[i],
		 fnode.node_offsets[i]*sizeof(fnode) );
	  search_tree( lat, lon, fnode.node_offsets[i]*sizeof(fnode), ft, fd, fv, fa);
	}
      }
    }
}
