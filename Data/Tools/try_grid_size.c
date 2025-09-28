//
// divide USA into grid of specified size in degrees
// histogram number of places in each grid square
// assign places to grids and write new file
//
//
// usage: try_grid_size <places_file> <d_lat> <d_lon>
//                      -h         output histogram
//                      -o <outf>  output file


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "load_places.h"

#define NHIST 1000
static int hist[NHIST];
static int histover;

static char buff[255];

int main( int argc, char *argv[]) {
  a_place* places;
  int numPlaces;
  double lat_min, lat_max, lon_min, lon_max;
  double lat_lo, lat_hi, lon_lo, lon_hi;  
  double lat_div, lon_div;

  int do_hist = 0;
  int verb = 0;
  char *outf = NULL;

  if( argc < 4) {
    printf("usage: ./try_grid_size <places> <d_lat> <d_lon> [-h] [-o file]\n");
    exit(1);
  }

  if( argc > 4) {
    for( int i=4; i<argc; i++) {
      if( *argv[i] == '-') {
	switch( toupper( argv[i][1])) {
	case 'H':
	  do_hist = 1;
	  break;
	case 'O':
	  if( i == argc-1) {
	    printf("Missing output file\n");
	  } else {
	    outf = argv[i+1];
	    ++i;
	  }
	  break;
	case 'V':
	  ++verb;
	  break;
	default:
	  printf("Unknown argument %s\n", argv[i]);
	  break;
	}
      } else {
	printf("Extra argument %s\n", argv[i]);
      }
    }
  }

  lat_div = atof( argv[2]);
  lon_div = atof( argv[3]);

  if( (places = load_places( argv[1], &numPlaces)) == NULL) {
    fprintf( stderr, "Error loading places from %s\n", argv[1]);
    exit(1);
  }

  printf("%d places loaded\n", numPlaces);

  // find min/max lat/lon
  lat_min = lat_max = places[0].lat;
  lon_min = lon_max = places[0].lon;

  for( int i=1; i<numPlaces; i++) {
    if( places[i].lat < lat_min)
      lat_min = places[i].lat;
    if( places[i].lat > lat_max)
      lat_max = places[i].lat;
    if( places[i].lon < lon_min)
      lon_min = places[i].lon;
    if( places[i].lon > lon_max)
      lon_max = places[i].lon;
  }

  printf("Latitude range: %f to %f\n", lat_min, lat_max);
  printf("Longitude range: %f to %f\n", lon_min, lon_max);

  lat_lo = lat_min - fmod(lat_min, lat_div) - lat_div;
  lon_lo = lon_min - fmod(lon_min, lon_div) - lon_div;

  lat_hi = lat_max - fmod(lat_max, lat_div) + lat_div;
  lon_hi = lon_max - fmod(lon_max, lon_div) + lon_div;  

  printf("Latitude  grid: %f to %f\n", lat_lo, lat_hi);
  printf("Longitude grid: %f to %f\n", lon_lo, lon_hi);

  int totalCheck = 0;
  int occupiedSquares = 0;

  int nlat;
  int nlon;
  double lat;
  double lon;

  int calc_nlat = (lat_hi-lat_lo)/lat_div + 1;
  int calc_nlon = (lon_hi-lon_lo)/lon_div + 1;

  printf("Calculated grid count nlat: %d  nlon: %d\n", calc_nlat, calc_nlon);
  printf("Range Lat: %g to %g step %g lon: %g to %g step: %g\n",
	 lat_lo, lat_hi, lat_div, lon_lo, lon_hi, lon_div);

  // for( double lat=lat_lo; lat <= lat_hi; lat += lat_div) {
  for( nlat = 0, lat=lat_lo; nlat < calc_nlat; nlat++, lat += lat_div) {
    if( verb)
      printf("Lat: %12.7g\n", lat);
    //    for( double lon=lon_lo; lon <= lon_hi; lon += lon_div) {
    for( lon=lon_lo, nlon = 0; nlon < calc_nlon; nlon++, lon += lon_div) {
      int numInSquare=0;
      for( int i=0; i<numPlaces; i++) {
	if( places[i].lat >= lat && places[i].lat < lat+lat_div &&
	    places[i].lon >= lon && places[i].lon < lon+lon_div) {
	  ++numInSquare;
	  places[i].used++;
	  places[i].lat_grid = nlat;
	  places[i].lon_grid = nlon;
	}
      }
      if( numInSquare) {
	//	printf("%4d lat %8.2f lon %8.2f\n", numInSquare, lat, lon);
	totalCheck += numInSquare;
	occupiedSquares++;
      }
      if( numInSquare < NHIST)
	hist[numInSquare]++;
      else
	histover++;
    }
  }

  if( numPlaces != totalCheck)
    fprintf( stderr, "Error!  loaded %d places but only counted %d in grid\n",
	     numPlaces, totalCheck);
    
  printf("%d squares occupied\n", occupiedSquares);
  printf("%d latitude grids, %d longitude grids (%d total)\n", nlat, nlon, nlat*nlon);
  for( int i=0; i<numPlaces; i++) {
    if( places[i].used != 1) {
      printf("Place %d used %d times! (should be 1!)\n", i, places[i].used);
      printf("%s, %s  %f  %f\n", places[i].name, places[i].state, places[i].lat, places[i].lon);
    }
  }
  if( do_hist) {
    for( int i=0; i<NHIST; i++)
      if( hist[i])
	printf("%4d: %d\n", i, hist[i]);
  }

  int hist_lo = NHIST;
  int hist_hi = 0;
  int hist_max = 0;
  int max_bin = 0;
  for( int i=1; i<NHIST; i++) {
    if( hist[i]) {
      if( i < hist_lo) hist_lo = i;
      if( i > hist_hi) hist_hi = i;
      if( hist[i] > hist_max) {
	hist_max = hist[i];
	max_bin = i;
      }
    }
  }
  printf("Hist range %d to %d max %d (at %d) overflow: %d\n", hist_lo, hist_hi, hist_max, max_bin, histover);

  // write output
  if( outf != NULL) {
    FILE *fo = fopen( outf, "wb");
    if( fo == NULL) {
      fprintf( stderr, "Can't open %s for writing\n", outf);
      exit(1);
    }

    for( int i=0; i<numPlaces; i++) {
      if( place_to_csv( &places[i], buff, sizeof(buff))) {
	fprintf( stderr, "Error writing place %d to CSV\n", i);
	exit(1);
      }
      fputs( buff, fo);
    }

    fclose( fo);

    // write grid info to stdout
    printf("--- Grid Info (save as grid.csv) ---\n");
    printf("NumLat, LatMin, LatMax, LatStep, NumLon, LonMin, LonMax, LonStep\n");
    printf("%d, %g, %g, %g, %d, %g, %g, %g\n", calc_nlat, lat_lo, lat_hi, lat_div,
	   calc_nlon, lon_lo, lon_hi, lon_div);
  }
}
