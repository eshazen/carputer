#include "distance_miles.h"

// Converts degrees to radians
static double deg2rad(double deg) {
    return deg * M_PI / 180.0;
}

/**
 * Computes the great-circle distance between two points on Earth.
 * @param lat1 Latitude of first point (degrees, signed)
 * @param lon1 Longitude of first point (degrees, signed)
 * @param lat2 Latitude of second point (degrees, signed)
 * @param lon2 Longitude of second point (degrees, signed)
 * @return Distance in miles.
 */
double distance_miles(double lat1, double lon1, double lat2, double lon2) {
    // Convert degrees to radians
    double lat1_rad = deg2rad(lat1);
    double lon1_rad = deg2rad(lon1);
    double lat2_rad = deg2rad(lat2);
    double lon2_rad = deg2rad(lon2);

    // Haversine formula
    double dlat = lat2_rad - lat1_rad;
    double dlon = lon2_rad - lon1_rad;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1_rad) * cos(lat2_rad) *
               sin(dlon / 2) * sin(dlon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    // Distance in miles
    return EARTH_RADIUS_MILES * c;
}
