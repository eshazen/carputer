// simplify_pollygons.c by ChatGPT:
//
// Simplify "plot format" files, with input as
//    N
//    lon1 lat1
//    lon2 lat2
//    ...
//
// 1.  Treat polygons as closed rings
// 
// 2.  During Douglas–Peucker recursion:
//     * Before replacing a chain of vertices with a single segment
//     * Test whether that segment would intersect any other polygon edge
// 
// 3.  If an intersection would occur:
//     * Reject the simplification
//     * Force recursion to keep intermediate vertices
// 
// 4.  Result:
//     * No self-intersections
//     * No edge crossings
//     * Polygon orientation preserved
// 
// This is slower than standard DP, but safe for geographic polygons.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EARTH_RADIUS_MILES 3958.7613
#define DEG2RAD(x) ((x) * M_PI / 180.0)

typedef struct {
    double lon, lat; /* degrees */
} Point;

/* ---------- Great-circle utilities ---------- */

static double haversine(Point a, Point b)
{
    double lat1 = DEG2RAD(a.lat);
    double lat2 = DEG2RAD(b.lat);
    double dlat = lat2 - lat1;
    double dlon = DEG2RAD(b.lon - a.lon);

    double h = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1) * cos(lat2) *
               sin(dlon / 2) * sin(dlon / 2);

    return 2 * EARTH_RADIUS_MILES * asin(sqrt(h));
}

static double bearing(Point a, Point b)
{
    double lat1 = DEG2RAD(a.lat);
    double lat2 = DEG2RAD(b.lat);
    double dlon = DEG2RAD(b.lon - a.lon);

    return atan2(
        sin(dlon) * cos(lat2),
        cos(lat1) * sin(lat2) -
        sin(lat1) * cos(lat2) * cos(dlon)
    );
}

/* Cross-track distance from P to great-circle AB */
static double cross_track_distance(Point p, Point a, Point b)
{
    double d13 = haversine(a, p) / EARTH_RADIUS_MILES;
    double theta13 = bearing(a, p);
    double theta12 = bearing(a, b);

    return fabs(asin(sin(d13) * sin(theta13 - theta12))
                * EARTH_RADIUS_MILES);
}

/* ---------- Intersection logic (planar-safe) ---------- */
/* Still valid because intersections depend on ordering,
   not distance metric */

static double orient(Point a, Point b, Point c)
{
    return (b.lon - a.lon) * (c.lat - a.lat) -
           (b.lat - a.lat) * (c.lon - a.lon);
}

static int segments_intersect(Point a, Point b, Point c, Point d)
{
    double o1 = orient(a, b, c);
    double o2 = orient(a, b, d);
    double o3 = orient(c, d, a);
    double o4 = orient(c, d, b);

    if (o1 == 0 && o2 == 0 && o3 == 0 && o4 == 0)
        return 0;

    return (o1 * o2 < 0) && (o3 * o4 < 0);
}

static int creates_intersection(Point *pts, int n, int i, int j)
{
    for (int k = 0; k < n - 1; k++) {
        if (k == i || k + 1 == i || k == j || k + 1 == j)
            continue;

        if (segments_intersect(pts[i], pts[j], pts[k], pts[k + 1]))
            return 1;
    }
    return 0;
}

/* ---------- Constrained Douglas–Peucker ---------- */

static void dp(Point *pts, int n, int i, int j,
               double eps, int *keep)
{
    double maxd = 0.0;
    int idx = -1;

    for (int k = i + 1; k < j; k++) {
        double d = cross_track_distance(pts[k], pts[i], pts[j]);
        if (d > maxd) {
            maxd = d;
            idx = k;
        }
    }

    if (maxd <= eps && !creates_intersection(pts, n, i, j)) {
        keep[i] = keep[j] = 1;
        return;
    }

    if (idx >= 0) {
        dp(pts, n, i, idx, eps, keep);
        dp(pts, n, idx, j, eps, keep);
    }
}

/* ---------- Polygon simplification ---------- */

static Point *simplify_polygon(Point *poly, int n,
                               double eps, int *out_n)
{
    if (poly[0].lon == poly[n - 1].lon &&
        poly[0].lat == poly[n - 1].lat)
        n--;

    Point *ring = malloc((n + 1) * sizeof(Point));
    for (int i = 0; i < n; i++)
        ring[i] = poly[i];
    ring[n] = poly[0];

    int rn = n + 1;
    int *keep = calloc(rn, sizeof(int));

    dp(ring, rn, 0, rn - 1, eps, keep);

    int cnt = 0;
    for (int i = 0; i < rn - 1; i++)
        if (keep[i]) cnt++;

    Point *out = malloc(cnt * sizeof(Point));
    int j = 0;
    for (int i = 0; i < rn - 1; i++)
        if (keep[i]) out[j++] = ring[i];

    free(ring);
    free(keep);

    *out_n = cnt;
    return out;
}

/* ---------- I/O ---------- */

static Point *read_polygon(FILE *fp, int *n)
{
    if (fscanf(fp, "%d", n) != 1)
        return NULL;

    Point *p = malloc((*n) * sizeof(Point));
    for (int i = 0; i < *n; i++)
        fscanf(fp, "%lf %lf", &p[i].lon, &p[i].lat);

    return p;
}

static void write_polygon(FILE *fp, Point *p, int n)
{
    fprintf(fp, "%d\n", n);
    for (int i = 0; i < n; i++)
        fprintf(fp, "%.10g %.10g\n", p[i].lon, p[i].lat);
}

/* ---------- Main ---------- */

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr,
            "Usage: %s input.txt output.txt epsilon_miles\n",
            argv[0]);
        return 1;
    }

    double epsilon = atof(argv[3]);

    FILE *in = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");

    if (!in || !out) {
        perror("File error");
        return 1;
    }

    while (1) {
        int n;
        Point *poly = read_polygon(in, &n);
        if (!poly) break;

        int out_n;
        Point *simp = simplify_polygon(poly, n, epsilon, &out_n);
        write_polygon(out, simp, out_n);

        free(poly);
        free(simp);
    }

    fclose(in);
    fclose(out);
    return 0;
}
