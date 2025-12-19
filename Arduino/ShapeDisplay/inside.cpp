
#include <stdio.h>
#include <SD.h>
#include "shape.h"

/*
 * Returns 1 if the point p is inside the polygon found by points in fv
 * otherwise returns 0.
 */
int point_in_part_file( File fv, int n, coord_t x, coord_t y)
{
  long pos;
  int i, j, inside = 0;
  a_point pt_i;
  a_point pt_j;

  pos = fv.position();

#ifdef DUMP_POINTS
  // dump the points
  Serial.print("point: ");
  Serial.print(x);
  Serial.print(",");
  Serial.println(y);

  for( i=0; i<n; i++) {
    fv.seek( pos+i*sizeof(a_point));
    // fread( &pt_i, sizeof(pt_i), 1, fv);
    fv.read( &pt_i, sizeof(pt_i));
    Serial.print(pt_i.lon);
    Serial.print(",");
    Serial.println(pt_i.lat);
  }
#endif
  
  for (i = 0, j = n - 1; i < n; j = i++) {

    // seek the file to offset i
    //fseek( fv, pos+i*sizeof(a_point), SEEK_SET);
    fv.seek( pos+i*sizeof(a_point));
    // fread( &pt_i, sizeof(pt_i), 1, fv);
    fv.read( &pt_i, sizeof(pt_i));
    coord_t xi = pt_i.lon, yi = pt_i.lat;

    // see the file to offset j
    // seek( fv, pos+j*sizeof(a_point), SEEK_SET);
    fv.seek( pos+j*sizeof(a_point));
    // read( &pt_j, sizeof(pt_j), 1, fv);
    fv.read( &pt_j, sizeof(pt_j));
    coord_t xj = pt_j.lon, yj = pt_j.lat;

    /* Check if edge intersects with the ray horizontally from p.x, p.y */
    int intersect = ((yi > y) != (yj > y)) &&
      (x < (xj - xi) * (y - yi) / (yj - yi + 0.0) + xi);
    if (intersect)
      inside = !inside;
  }

  // fseek( fv, pos, SEEK_SET);
  fv.seek( pos);

  return inside;
}

