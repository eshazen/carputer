
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #define DEBUG

// #define VERBOSE

// use new polygon format for VRT file
//
//    <nvert>
//      <lat> <lat>...
//      <lon> <lon>...
//        ...
//    <nvert>
//        ...
//    0	

#define NEW_POLY
#include "shape.h"

//
// print one shape for debug
// (ignore coordinate list)
//
void print_shape( a_shape* s) {
  fprintf( stderr, "SHAPE: %s (%d) ", s->name, s->nvert);
  fprintf( stderr, "Lat (%f..%f) Lon: (%f..%f)\n",
	 s->minLat, s->maxLat, s->minLon, s->maxLon);
#ifdef VERBOSE
  for( int i=0; i<s->nvert; i++) {
    fprintf( stderr, "  vert %d: (%f, %f)\n", i, s->points[i].lat, s->points[i].lon);
  }
#endif
}


void print_fshape( f_shape* s) {
  fprintf( stderr, "FSHAPE: %s (%d) ", s->name, s->nvert);
  fprintf( stderr, "Points off: %d\n", s->points_off);
#ifdef INT_COORD
  fprintf( stderr, "Lat (%d..%d) Lon: (%d..%d)\n",
	 s->minLat, s->maxLat, s->minLon, s->maxLon);
#else  
  fprintf( stderr, "Lat (%f..%f) Lon: (%f..%f)\n",
	 s->minLat, s->maxLat, s->minLon, s->maxLon);
#endif
  fprintf( stderr, "  %d parts offset %d\n", s->nparts, s->part_off);
}

void shape_to_f( f_shape* fs, a_shape* ms) {
  fs->nvert = ms->nvert;
  strncpy( fs->name, ms->name, MAX_NAME);
#ifdef INT_COORD
  fs->minLat = COORD_I_SCALE * ms->minLat;
  fs->minLon = COORD_I_SCALE * ms->minLon;
  fs->maxLat = COORD_I_SCALE * ms->maxLat;
  fs->maxLon = COORD_I_SCALE * ms->maxLon;
#else
  fs->minLat = ms->minLat;
  fs->minLon = ms->minLon;
  fs->maxLat = ms->maxLat;
  fs->maxLon = ms->maxLon;
#endif  
  fs->points_off = 0;
  fs->nparts = ms->nparts;
}

// write shape data to three files
// DAT file has the following format:
//   int ns
//   f_shape shapes[ns]
// VRT file has all the virtex arrays in order for the shapes

int32_t write_shapes( a_shape* shapes, int32_t nshape, FILE *fp, FILE *fv, FILE *fa) {
  int32_t cpos = 0;		// offset in virtex file
  int32_t ppos = 0;		/* offset in parts file */

  f_shape fshapes[nshape];	/* allocate an array for the file format shapes */

  for( int i=0; i<nshape; i++) {
    shape_to_f( &fshapes[i], &shapes[i]); /* copy the array */

    // write parts
    fshapes[i].part_off = ftell( fa);
#ifdef DEBUG
    fprintf( stderr, "parts at %d:\n", fshapes[i].part_off);
#endif
    for( int k=0; k<shapes[i].nparts; k++) {
#ifdef DEBUG
      fprintf( stderr, "  %d %d\n", k, shapes[i].parts[k]);
#endif
      fwrite( &shapes[i].parts[k], sizeof(uint32_t), 1, fa);
    }

#ifdef DEBUG
    fprintf( stderr, "num:%d %s nvert = %d\n", i, shapes[i].name, shapes[i].nvert);
    print_shape( &shapes[i]);
    print_fshape( &fshapes[i]);
#endif    

    int32_t llsiz = sizeof( coord_t) * shapes[i].nvert; // lat/lon list sizes
#ifdef DEBUG
    fprintf( stderr, " LATLON: llsiz=%" PRId32 " cpos=%" PRId32 "\n", llsiz, cpos);
    fprintf( stderr, "Writing %d points\n", shapes[i].nvert);
    for( int k=0; k<shapes[i].nvert; k++) {
      if( k < 5)
	fprintf( stderr,  "  write virtex: %f %f\n", shapes[i].points[k].lat, shapes[i].points[i].lon);
      ;
    }
#endif    
    fwrite( shapes[i].points, sizeof( a_point), shapes[i].nvert, fv);
    fshapes[i].points_off = cpos;
    cpos += llsiz * 2;
#ifdef DEBUG    
    fprintf( stderr, " LLEND: cpos = %" PRId32 "\n", cpos);
#endif
  }
  
  fwrite( &nshape, sizeof(int32_t), 1, fp);
  fwrite( fshapes, sizeof(f_shape), nshape, fp);
}
