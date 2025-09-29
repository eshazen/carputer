#include <math.h>

#define EARTH_RADIUS_MILES 3958.8  // Mean Earth radius in miles

double distance_miles(double lat1, double lon1, double lat2, double lon2);
static double deg2rad(double deg);
