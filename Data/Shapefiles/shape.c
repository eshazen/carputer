
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// NOTE NOTE NOTE:  NEW_PART defined in Makefile!
// #define NEW_PART
// #define DEBUG
// #define VERBOSE

#include "shape.h"
#include "fileset.h"

//
// print one shape for debug
// (ignore coordinate list)
//
void print_shape( a_shape* s) {
  printf(  "SHAPE: %s (%d) ", s->name, s->nvert);
  printf(  "Lat (%f..%f) Lon: (%f..%f)\n",
	 s->minLat, s->maxLat, s->minLon, s->maxLon);
#ifdef VERBOSE
  for( int i=0; i<s->nvert; i++) {
    printf(  "  vert %d: (%f, %f)\n", i, s->points[i].lat, s->points[i].lon);
  }
#endif
}


void print_fshape( f_shape* s) {
  printf("FSHAPE: %s prio=%d\n", s->name, s->prio);
  printf("  Points off: %d  count: %d\n", s->points_off, s->nvert);
  printf("  Parts off:  %d  count: %d\n", s->part_off, s->nparts);
#ifdef INT_COORD
  printf("  Lat (%d..%d) Lon: (%d..%d)\n",
	 s->minLat, s->maxLat, s->minLon, s->maxLon);
#else  
  printf("  Lat (%f..%f) Lon: (%f..%f)\n",
	 s->minLat, s->maxLat, s->minLon, s->maxLon);
#endif
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

int32_t write_shape_fileset( a_shape* shapes, int32_t nshape, a_fileset fs) {
  int32_t cpos = 0;		// offset in virtex file
  int32_t ppos = 0;		/* offset in parts file */

  f_shape fshapes[nshape];	/* allocate an array for the file format shapes */

  for( int i=0; i<nshape; i++) {
    shape_to_f( &fshapes[i], &shapes[i]); /* copy the array */

    // write parts
    fshapes[i].part_off = ftell( fs.prt);
#ifdef DEBUG
    printf("parts at %d:\n", fshapes[i].part_off);
#endif

// NOTE NOTE NOTE:  NEW_PART defined in Makefile!
#ifdef NEW_PART
    a_part* part_list = make_part_list( shapes[i].nparts, shapes[i].parts, shapes[i].nvert);
    if( part_list != NULL) {
      fwrite( part_list, sizeof(a_part), shapes[i].nparts, fs.prt);
      free( part_list);
    }
#else

    for( int k=0; k<shapes[i].nparts; k++) {
#ifdef DEBUG
      printf("  %d %d\n", k, shapes[i].parts[k]);
#endif
      fwrite( &shapes[i].parts[k], sizeof(uint32_t), 1, fs.prt);
    }
#endif
    
#ifdef DEBUG
    printf("num:%d %s nvert = %d\n", i, shapes[i].name, shapes[i].nvert);
    print_shape( &shapes[i]);
    print_fshape( &fshapes[i]);
#endif    

    int32_t llsiz = sizeof( coord_t) * shapes[i].nvert; // lat/lon list sizes
#ifdef DEBUG
    printf(" LATLON: llsiz=%" PRId32 " cpos=%" PRId32 "\n", llsiz, cpos);
    printf("Writing %d points\n", shapes[i].nvert);
    for( int k=0; k<shapes[i].nvert; k++) {
      if( k < 5)
	printf( "  write virtex: %f %f\n", shapes[i].points[k].lat, shapes[i].points[i].lon);
      ;
    }
#endif    
    fwrite( shapes[i].points, sizeof( a_point), shapes[i].nvert, fs.vrt);
    fshapes[i].points_off = cpos;
    cpos += llsiz * 2;
#ifdef DEBUG    
    printf(" LLEND: cpos = %" PRId32 "\n", cpos);
#endif
  }
  
  fwrite( &nshape, sizeof(int32_t), 1, fs.dat);
  fwrite( fshapes, sizeof(f_shape), nshape, fs.dat);
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
    printf("parts at %d:\n", fshapes[i].part_off);
#endif
    for( int k=0; k<shapes[i].nparts; k++) {
#ifdef DEBUG
      printf("  %d %d\n", k, shapes[i].parts[k]);
#endif
      fwrite( &shapes[i].parts[k], sizeof(uint32_t), 1, fa);
    }

#ifdef DEBUG
    printf("num:%d %s nvert = %d\n", i, shapes[i].name, shapes[i].nvert);
    print_shape( &shapes[i]);
    print_fshape( &fshapes[i]);
#endif    

    int32_t llsiz = sizeof( coord_t) * shapes[i].nvert; // lat/lon list sizes
#ifdef DEBUG
    printf(" LATLON: llsiz=%" PRId32 " cpos=%" PRId32 "\n", llsiz, cpos);
    printf("Writing %d points\n", shapes[i].nvert);
    for( int k=0; k<shapes[i].nvert; k++) {
      if( k < 5)
	printf( "  write virtex: %f %f\n", shapes[i].points[k].lat, shapes[i].points[i].lon);
      ;
    }
#endif    
    fwrite( shapes[i].points, sizeof( a_point), shapes[i].nvert, fv);
    fshapes[i].points_off = cpos;
    cpos += llsiz * 2;
#ifdef DEBUG    
    printf(" LLEND: cpos = %" PRId32 "\n", cpos);
#endif
  }
  
  fwrite( &nshape, sizeof(int32_t), 1, fp);
  fwrite( fshapes, sizeof(f_shape), nshape, fp);
}



//
// convert a list of part starting offsets to (count,offset)
//
// return malloc'd list of a_part
// bail out on errors
//
a_part* make_part_list( int nparts, uint32_t *parts, uint32_t nvert) {
  a_part* p;

  if( nparts <1 ) {
    printf(  "ERROR:  zero parts\n");
    // try just returning NULL
    return NULL;
  }

  p = calloc( nparts, sizeof(a_part));

  // simplest case, only one part
  p[0].count = nvert;
  p[0].offset = 0;
  if( nparts == 1)
    return p;

  // more than one part
  for( int i=0; i<nparts; i++) {
    if( i == nparts-1) {
      p[i].count = nvert-parts[i];
    } else {
      p[i].count = parts[i+1]-parts[i];
      p[i+1].offset = p[i].offset + p[i].count;
    }
  }

  return p;
}

