//
// find closest places to specified lat/long
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "parse_csv.h"

#define EARTH_RADIUS_MILES 3958.8  // Mean Earth radius in miles

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

char buff[120];
#define MAXTOKENS 10
static char *tokens[MAXTOKENS];

int main( int argc, char *argv[]) {
  double lat = atof( argv[1]);
  double lon = atof( argv[2]);

  FILE *fp;

  if( (fp = fopen( "places.csv", "r")) == NULL) {
    printf("Can't open places.csv\n");
    exit(1);
  }

  while( fgets( buff, sizeof(buff), fp)) {

    int nf = parse_csv_line( buff, tokens, MAXTOKENS);

    char* name = tokens[1];
    char* abbr = tokens[2];
    double plat = atof( tokens[3]);
    double plon = atof( tokens[4]);

    double dist = distance_miles( plat, plon, lat, lon);
    printf( "%8.1f %30s %2s\n", dist, name, abbr);
  }
}
