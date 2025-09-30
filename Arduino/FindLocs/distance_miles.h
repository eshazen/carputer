
#include <math.h>

#define EARTH_RADIUS_MILES 3958.8  // Mean Earth radius in miles


// Converts degrees to radians
static double deg2rad(double deg) {
    return deg * M_PI / 180.0;
}

double distance_miles(double lat1, double lon1, double lat2, double lon2);
