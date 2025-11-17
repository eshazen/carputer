#ifndef _SHAPE_H_DEFINED
#define _SHAPE_H_DEFINED

#include <stdio.h>
#include <stdint.h>

// define the type to use for lat/lon coordinates
typedef float coord_t;

#define MAX_NAME 80

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

typedef struct {
  int nvert;
  char name[MAX_NAME];
  uint32_t lat_off;
  uint32_t lon_off;
  coord_t minLat;
  coord_t minLon;
  coord_t maxLat;
  coord_t maxLon;
} f_shape;

void print_shape( a_shape* shape);
void print_fshape( f_shape* shape);
int32_t write_shapes( a_shape* shapes, int32_t nshape, FILE *fp, FILE *fv);

#endif
