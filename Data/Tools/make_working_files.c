//
// create all the working files for the car computer
//
// divide USA into grid of specified size in degrees
// histogram number of places in each grid square
// assign places to grids
//
// read places file, with
//   type, name, state, lat, long
//
//    1,"Fernandina Beach",FL, +30.6579158, -081.4500012
//    1,"Orlando",FL, +28.4728185, -081.3202421
//    1,"Zolfo Springs",FL, +27.4925788, -081.7867927
//
// get grid spacing from command-line
// create a grid large enough to contain all the places
// assign each place to a grid by (x,y) where x and y
// are integer grid square numbers in lat and lon.
//
// sort places by grid coordinates, in order by longitude,
// then latitude
//
// write out new places.csv file with grid coordinates added
// write an info file grid.csv with columns:
//   NumLat, LatMin, LatMax, LatStep, NumLon, LonMin, LonMax, LonStep
// write an index file gindex.dat with the (32-bit binary)
// file offset of the start of each group in places by grid location
// offset set to a sentinel value (e.g. -1) if grid is empty
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>

#include "load_places.h"
#include "file_util.h"

// ---- hardwired (ugh) output file names ----
#define PLACES_FILE "places.csv"
#define GRID_FILE "grid.csv"
#define INDEX_FILE "gindex.dat"

// list
static char* filez[] = { PLACES_FILE, GRID_FILE, INDEX_FILE};

#define NHIST 1000
static int hist[NHIST];
static int histover;

static char buff[255];

// big binary table with offsets to start of each grid area
// or -1 if none.  First subscript is latitude
static int32_t gridIndex[256][256];

//
// function to compare lat/lon grid assignments
// lat_grid, lon_grid for qsort()
//
int cmp_places( const void* a, const void* b) {
  const a_place* pa = (a_place*)a;
  const a_place* pb = (a_place*)b;

  if( pa->lat_grid != pb->lat_grid)
    return pa->lat_grid < pb->lat_grid ? -1 : 1;

  if( pa->lon_grid == pb->lon_grid)
    return 0;

  return pa->lon_grid < pb->lon_grid ? -1 : 1;
}

//---------- main ----------

int main( int argc, char *argv[]) {
  a_place* places;
  int numPlaces;
  double lat_min, lat_max, lon_min, lon_max;
  double lat_lo, lat_hi, lon_lo, lon_hi;  
  double lat_div, lon_div;

  int do_hist = 0;
  int verb = 0;
  char *outd = NULL;

  if( argc < 4) {
    printf("usage: ./try_grid_size <places> <d_lat> <d_lon> [-h] [-o dir]\n");
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
	    printf("Missing output path\n");
	  } else {
	    outd = argv[i+1];
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

  if( outd != NULL) {
    FILE* ft;

    // see if directory exists
    DIR* dir = opendir( outd);
    if( dir) {
      closedir(dir);
    } else {
      fprintf( stderr, "Error: directory %s doesn't exist\n", outd);
      exit(1);
    }
    chdir( outd);

    // check for clobber output files
    if( check_clobber( filez, 3)) {
      printf("OK to clobber existing files! (y/n): ");
      fgets( buff, 10, stdin);
      if( toupper(*buff) != 'Y')
	exit(1);
    }

  }

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

  int numLat = (lat_hi-lat_lo)/lat_div + 1;
  int numLon = (lon_hi-lon_lo)/lon_div + 1;

  printf("Calculated grid count nlat: %d  nlon: %d\n", numLat, numLon);
  printf("Range Lat: %g to %g step %g lon: %g to %g step: %g\n",
	 lat_lo, lat_hi, lat_div, lon_lo, lon_hi, lon_div);

  if( numLat > 255 || numLon > 255) {
    fprintf( stderr, "Error:  grid count in lat/lon must be < 255\n");
    exit(1);
  }

  printf("Clear the grid\n");
  // set grid index to all -1
  for( int i=0; i<256; i++)
    for( int j=0; j<256; j++)
      gridIndex[i][j] = -1;

  printf("Set up the grid\n");
  // for( double lat=lat_lo; lat <= lat_hi; lat += lat_div) {
  for( nlat = 0, lat=lat_lo; nlat < numLat; nlat++, lat += lat_div) {
    //    for( double lon=lon_lo; lon <= lon_hi; lon += lon_div) {
    for( lon=lon_lo, nlon = 0; nlon < numLon; nlon++, lon += lon_div) {
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

  // sort places by lat_grid, lon_grid
  qsort( places, numPlaces, sizeof( a_place), cmp_places );

  // places file
  if( outd != NULL) {

    FILE *fo = fopen( PLACES_FILE, "wb");
    if( fo == NULL) {
      fprintf( stderr, "Can't open %s for writing %s\n", PLACES_FILE, outd);
      exit(1);
    }

    printf("Writing %d places to %s\n", numPlaces, PLACES_FILE);

    for( int i=0; i<numPlaces; i++) {
      if( place_to_csv( &places[i], buff, sizeof(buff))) {
	fprintf( stderr, "Error writing place %d to CSV\n", i);
	exit(1);
      }
      if( verb && i < 10)
	fputs( buff, stdout);
      if( fputs( buff, fo) == EOF) {
	fprintf( stderr, "Error writing to %s\n", PLACES_FILE);
	exit(1);
      }
    }

    long lastp = ftell(fo);
    printf("places end: %ld\n", lastp);

    if( fclose( fo)) {
      printf("Error closing %s\n", PLACES_FILE);
      perror(NULL);
    }



    // write grid info to stdout
    printf("--- Grid Info (save as grid.csv) ---\n");
    printf("NumLat, LatMin, LatMax, LatStep, NumLon, LonMin, LonMax, LonStep\n");
    printf("%d, %g, %g, %g, %d, %g, %g, %g\n", numLat, lat_lo, lat_hi, lat_div,
	   numLon, lon_lo, lon_hi, lon_div);

    // write to file grid.csv
    fo = fopen( GRID_FILE, "wb");
    if( fo == NULL) {
      fprintf( stderr, "Can't open %s for writingin %s\n", GRID_FILE, outd);
      exit(1);
    }

    fprintf( fo, "NumLat, LatMin, LatMax, LatStep, NumLon, LonMin, LonMax, LonStep\n");
    fprintf( fo, "%d, %g, %g, %g, %d, %g, %g, %g\n", numLat, lat_lo, lat_hi, lat_div,
	   numLon, lon_lo, lon_hi, lon_div);
    fclose( fo);
    
    // create the binary index
    // best to read the file to be sure the positions are correct
    long posn = 0L;
    int npos = 0;
    a_place tp, tp0;
    fo = fopen( INDEX_FILE, "wb");

    // We want to keep the previous place in tp0, current one in tp
    // setup before loop
    if( csv_to_place( &tp0, buff)) {
      fprintf( stderr, "Re-read places... Error in line: %s\n", buff);
      exit(1);
    }

    while( fgets( buff, sizeof(buff), fo)) {

      csv_to_place( &tp, buff);	/* next place to tp */
      // check for change in grid posn
      if( verb && npos < 100)
	printf("Check (%d,%d) vs (%d,%d)\n", tp.lat_grid, tp.lon_grid,
	       tp0.lat_grid, tp0.lon_grid);
      if( tp.lat_grid != tp0.lat_grid || tp.lon_grid != tp0.lon_grid) {
	// record the new position
	gridIndex[tp.lat_grid][tp.lon_grid] = posn;
	if( verb && npos < 50)
	  printf("Set index[%d][%d] = %ld\n", tp.lat_grid, tp.lon_grid, posn);
      }

      free_place( &tp0);

      tp0 = tp;
      posn = ftell( fo);
      ++npos;
    }
    fclose(fo);

    if( (fo = fopen( INDEX_FILE, "wb")) == NULL) {
      fprintf( stderr, "Error opening %s\n", INDEX_FILE);
      exit(1);
    }

    size_t nw = fwrite( gridIndex, sizeof(int32_t), 256*256, fo);

    fclose(fo);

    // dump the gridIndex
    if( verb) {
      for( int ilat = 0; ilat < 256; ilat++)
	for( int ilon = 0; ilon < 256; ilon++)
	  if( gridIndex[ilat][ilon] > 0) {
	    printf("(%d,%d) = %d\n", ilat, ilon, gridIndex[ilat][ilon]);
	  }
    }

  }

}
