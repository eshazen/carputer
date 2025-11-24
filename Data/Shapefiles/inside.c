
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
