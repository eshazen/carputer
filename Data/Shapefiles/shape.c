
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DEBUG

#include "shape.h"

//
// print one shape for debug
// (ignore coordinate list)
//
void print_shape( a_shape* s) {
  printf("%s (%d) ", s->name, s->nvert);
  printf("Lat (%f..%f) Lon: (%f..%f)\n",
	 s->minLat, s->maxLat, s->minLon, s->maxLon);
}


void print_fshape( f_shape* s) {
  printf("%s (%d) ", s->name, s->nvert);
#ifdef INT_COORD
  printf("Lat (%d..%d) Lon: (%d..%d)\n",
	 s->minLat, s->maxLat, s->minLon, s->maxLon);
#else  
  printf("Lat (%f..%f) Lon: (%f..%f)\n",
	 s->minLat, s->maxLat, s->minLon, s->maxLon);
#endif
}

void shape_to_f( f_shape* fs, a_shape* ms) {
  fs->nvert = ms->nvert;
  strncpy( fs->name, ms->name, MAX_NAME);
  fs->minLat = COORD_I_SCALE * ms->minLat;
  fs->minLon = COORD_I_SCALE * ms->minLon;
  fs->maxLat = COORD_I_SCALE * ms->maxLat;
  fs->maxLon = COORD_I_SCALE * ms->maxLon;
  fs->lat_off = 0;
  fs->lon_off = 0;
}

// write shape data to two files
// DAT file has the following format:
//   int ns
//   f_shape shapes[ns]
// VRT file has all the virtex arrays in order for the shapes

int32_t write_shapes( a_shape* shapes, int32_t nshape, FILE *fp, FILE *fv) {
  int32_t shapsiz = sizeof( a_shape) * nshape + sizeof(int);
  int32_t cpos = 0;		// offset in virtex file

  f_shape fshapes[nshape];	/* allocate an array for the file format shapes */

#ifdef DEBUG
  printf("SHAPSIZ = %" PRId32 " (each: %ld)\n", shapsiz, sizeof( a_shape));
#endif  

  for( int i=0; i<nshape; i++) {
    shape_to_f( &fshapes[i], &shapes[i]); /* copy the array */

#ifdef DEBUG
    printf("SHAPE: num:%d %s nvert = %d\n", i, shapes[i].name, shapes[i].nvert);
    print_shape( &shapes[i]);
    print_fshape( &fshapes[i]);
#endif    
    int32_t llsiz = sizeof( coord_t) * shapes[i].nvert; // lat/lon list sizes
#ifdef DEBUG
    printf(" LATLON: llsiz=%" PRId32 " cpos=%" PRId32 "\n", llsiz, cpos);
#endif    
    fwrite( shapes[i].lat, sizeof( coord_t), shapes[i].nvert, fv);
    fshapes[i].lat_off = cpos;
    cpos += llsiz;
    fwrite( shapes[i].lon, sizeof( coord_t), shapes[i].nvert, fv);
    fshapes[i].lon_off = cpos;
    cpos += llsiz;
#ifdef DEBUG    
    printf(" LLEND: cpos = %" PRId32 "\n", cpos);
#endif
  }
  
  fwrite( &nshape, sizeof(int32_t), 1, fp);
  fwrite( fshapes, sizeof(f_shape), nshape, fp);
}
