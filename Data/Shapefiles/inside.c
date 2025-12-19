
#include <stdio.h>
#include "shape.h"

// typedef struct {
//     double x;
//     double y;
// } Point;

/*
 * Returns 1 if the point p is inside the polygon defined by vertices[] 
 * (an array of length n), otherwise returns 0.
 */
// int point_in_polygon(const Point *vertices, int n, Point p)
int point_in_polygon( const coord_t *xvert, const coord_t *yvert, int n, coord_t x, coord_t y)
{
    int i, j, inside = 0;

    for (i = 0, j = n - 1; i < n; j = i++) {
      coord_t xi = xvert[i], yi = yvert[i];
      coord_t xj = xvert[j], yj = yvert[j];

        /* Check if edge intersects with the ray horizontally from p.x, p.y */
        int intersect = ((yi > y) != (yj > y)) &&
                        (x < (xj - xi) * (y - yi) / (yj - yi + 0.0) + xi);
        if (intersect)
            inside = !inside;
    }

    return inside;
}


/*
 * Returns 1 if the point p is inside the polygon defined by vertices[] 
 * (an array of length n), otherwise returns 0.
 */
// int point_in_polygon(const Point *vertices, int n, Point p)
int point_in_part( const a_point* verts, int n, coord_t x, coord_t y)
{
    int i, j, inside = 0;

    for (i = 0, j = n - 1; i < n; j = i++) {
      coord_t xi = verts[i].lon, yi = verts[i].lat;
      coord_t xj = verts[j].lon, yj = verts[j].lat;

        /* Check if edge intersects with the ray horizontally from p.x, p.y */
        int intersect = ((yi > y) != (yj > y)) &&
                        (x < (xj - xi) * (y - yi) / (yj - yi + 0.0) + xi);
        if (intersect)
            inside = !inside;
    }

    return inside;
}

/*
 * Returns 1 if the point p is inside the polygon found by points in fv
 * otherwise returns 0.
 */
int point_in_part_file( FILE *fv, int n, coord_t x, coord_t y)
{
  long pos;
  int i, j, inside = 0;
  a_point pt_i;
  a_point pt_j;

  pos = ftell(fv);

  for (i = 0, j = n - 1; i < n; j = i++) {

    // seek the file to offset i
    fseek( fv, pos+i*sizeof(a_point), SEEK_SET);
    fread( &pt_i, sizeof(pt_i), 1, fv);
    coord_t xi = pt_i.lon, yi = pt_i.lat;

    // see the file to offset j
    fseek( fv, pos+j*sizeof(a_point), SEEK_SET);
    fread( &pt_j, sizeof(pt_j), 1, fv);
    coord_t xj = pt_j.lon, yj = pt_j.lat;

    /* Check if edge intersects with the ray horizontally from p.x, p.y */
    int intersect = ((yi > y) != (yj > y)) &&
      (x < (xj - xi) * (y - yi) / (yj - yi + 0.0) + xi);
    if (intersect)
      inside = !inside;
  }

  fseek( fv, pos, SEEK_SET);

  return inside;
}

