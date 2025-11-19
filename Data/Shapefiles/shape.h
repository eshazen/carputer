#ifndef _SHAPE_H_DEFINED
#define _SHAPE_H_DEFINED

#include <stdio.h>
#include <stdint.h>

#define INT_COORD

// define the type to use for lat/lon coordinates
typedef float coord_t;
#ifdef INT_COORD
typedef int32_t coord_i;
#else
typedef float coord_i;
#endif

#define COORD_I_SCALE 1000.0

#define MAX_NAME 80

// in-memory shape with pointers
typedef struct {
  int nvert;
  char name[MAX_NAME];
  coord_t *lat;
  coord_t *lon;
  coord_t minLat;
  coord_t minLon;
  coord_t maxLat;
  coord_t maxLon;
} a_shape;

// file shape with offsets
typedef struct {
  int32_t nvert;
  char name[MAX_NAME];
  uint32_t lat_off;
  uint32_t lon_off;
  coord_i minLat;
  coord_i minLon;
  coord_i maxLat;
  coord_i maxLon;
} f_shape;

void print_shape( a_shape* shape);
void print_fshape( f_shape* shape);
int32_t write_shapes( a_shape* shapes, int32_t nshape, FILE *fp, FILE *fv);

#endif
