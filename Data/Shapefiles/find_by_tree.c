//
// lookup a lat/long using R-Tree and location from files
//

#include <stdio.h>

#include "filetree.h"
#include "inside.c"

static char buff[80];

int verbose = 0;

void search_tree( coord_t lat, coord_t lon, long offset, FILE *ft, FILE *fd, FILE *fv, FILE *fa);

int main( int argc, char *argv[]) {

  FILE *ft;
  FILE *fd;
  FILE *fv;
  FILE *fa;
  coord_t lat, lon;

  if( argc < 4) {
    fprintf( stderr, "usage: %s fileset lat lon\n", argv[0]);
    return 1;
  }

  lat = atof( argv[2]);
  lon = atof( argv[3]);

  snprintf( buff, sizeof(buff), "%s.REE", argv[1]);
  ft = fopen( buff, "rb");

  snprintf( buff, sizeof(buff), "%s.DAT", argv[1]);
  fd = fopen( buff, "rb");

  snprintf( buff, sizeof(buff), "%s.VRT", argv[1]);
  fv = fopen( buff, "rb");

  snprintf( buff, sizeof(buff), "%s.PRT", argv[1]);
  fa = fopen( buff, "rb");

  if( !fv || !ft || !fd || !fa) {
    fprintf( stderr, "Couldn't open %s.REE, .DAT, .VRT or .PRT\n", argv[1]);
    return 1;
  }

  search_tree( lat, lon, 0L, ft, fd, fv, fa);		/* start at root */
}


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

  printf("NODE: %ld type=%d count=%d\n", offset, fnode.kind, fnode.count);
    // check against rects
    for( int i=0; i<fnode.count; i++) {
//      printf("Compare lat %f vs (%f..%f) and lon %f vs (%f..%f)\n",
//	     lat, fnode.rects[i].min[0], fnode.rects[i].max[0],
//	     lon, fnode.rects[i].min[1], fnode.rects[i].max[1]);
      if( lat >= fnode.rects[i].min[0] && lat <= fnode.rects[i].max[0] &&
	  lon >= fnode.rects[i].min[1] && lon <= fnode.rects[i].max[1]) {
	printf("MATCH at index %d... ", i);
	if( fnode.kind == LEAF) { /* LEAF */
	  printf("LEAF: %ld...", fnode.item_offsets[i]);
	  fseek( fd, fnode.item_offsets[i], SEEK_SET);
	  fread( &fshape, sizeof(fshape), 1, fd);
	  // now point to the virtex list
	  //	  printf("Reading %d verteces\n", fshape.nvert);
	  fseek( fv, fshape.part_off, SEEK_SET);
	  a_point* point_ptr = calloc( fshape.nvert, sizeof(coord_t));
	  //	  coord_t* lat_ptr = calloc( fshape.nvert, sizeof(coord_t));
	  //	  coord_t* lon_ptr = calloc( fshape.nvert, sizeof(coord_t));
	  fread( point_ptr, sizeof(a_point), fshape.nvert, fv);
	  //	  fread( lat_ptr, sizeof(coord_t), fshape.nvert, fv);
	  //	  fread( lon_ptr, sizeof(coord_t), fshape.nvert, fv);
	  if( verbose) {
	    for( int k=0; k<fshape.nvert; k++) {
	      printf("(%f,%f) ", point_ptr[k].lat, point_ptr[k].lon);
	      if( (k % 10) == 0)
		printf("\n");
	    }
	    printf("\n");
	  }

	  printf("LEAF SHAPE:\n");
	  print_fshape( &fshape);

	  // loop over parts
	  if( fseek( fa, fshape.part_off, SEEK_SET)) {
	    fprintf( stderr, "Error seeking to offset %d in PRT file\n", fshape.part_off);
	  }

	  uint32_t p_start, p_end;
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
	    fprintf( stderr, "Part %d from %d to %d\n", j, p_start, p_end);
	    for( unsigned k=p_start; k<p_end; k++) {
	      
	    }
	  }

//	  if( point_in_polygon( lat_ptr, lon_ptr, fshape.nvert, lat, lon))
//	    printf("is INSIDE!\n");
//	  else
//	    printf("outside\n");


	  free( point_ptr);
	} else {		  /* BRANCH */
	  printf("BRANCH: go down to node %ld offset %ld\n", fnode.node_offsets[i],
		 fnode.node_offsets[i]*sizeof(fnode) );
	  search_tree( lat, lon, fnode.node_offsets[i]*sizeof(fnode), ft, fd, fv, fa);
	}
      }
    }
}
