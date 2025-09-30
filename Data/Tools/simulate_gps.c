//
// Simulation code for the embedded device
// (could eventually be ported over)
//
// take gps lat/long from the command line and find n nearest locations
// use files:
//   places.csv    - places annotated with grid coordinates
//   grid.csv      - one line of grid spacing/range info
//   gindex.dat    - 256x256 array of 32-bit binary integers
//                   with offsets to start of each grid cell

// ---- hardwired (ugh) input file names ----
#define PLACES_FILE "places.csv"
#define GRID_FILE "grid.csv"
#define INDEX_FILE "gindex.dat"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "parse_csv.h"
#include "places.h"
#include "distance_miles.h"

static char buff[80];
static char* tokn[10];
#define MAXT (sizeof(tokn)/sizeof(tokn[0]))

static FILE *fp;
static FILE *fg;
static FILE *fi;

// structure for an in-memory location
typedef struct {
  uint32_t offset;		/* offset in places file */
  float distance;		/* distance to our GPS location */
} a_loc;

// array of up to 100 nearby locations from our grid cell
// and neighboring ones
static a_loc locs[1000];
#define MAXLOC (sizeof(locs)/sizeof(a_loc))
static int numLoc;		/* pointer to last occupied in locs */

// sort by location
int cmpf( const void* a, const void* b) {
  a_loc* pa = (a_loc *)a;
  a_loc* pb = (a_loc *)b;
  float v1 = pa->distance;
  float v2 = pb->distance;
  int rc;
  if( v1 == v2)
    rc = 0;
  else {
    if( v1 - v2 < 0)
      rc = -1;
    else
      rc = 1;
  }
  return rc;
}

//
// process a grid 
int process_grid_at( float lat, float lon, int latGrid, int lonGrid, a_loc* pary, int maxp) {

  // check for out of range
  if( latGrid < 0 || lonGrid < 0 || lonGrid > 255 || latGrid > 255)
    return 0;

  int nloc = 0;
  a_place pl;

  uint32_t posn = ((latGrid << 8) | lonGrid) * sizeof( uint32_t);
  printf("posn (0x%02x, 0x%02x) = 0x%04x (%d)\n", latGrid, lonGrid, posn, posn);
  fseek( fi, posn, SEEK_SET);

  uint32_t offset;
  fread( &offset, 1, sizeof(offset), fi);
  printf( "Retrieved offset 0x%04x from position %d\n", offset, posn);

  if( offset != 0xffffff) {

    printf("Seeking to offset\n");

    fseek( fp, offset, SEEK_SET);
    uint32_t offs0 = offset;

    // read until EOF or grid changes
    while( fgets( buff, sizeof(buff), fp)) {
      offset = ftell( fp);
      if( csv_to_place( &pl, buff)) {
	printf("Error parsing %s\n", buff);
	exit(1);
      }
      if( pl.lat_grid == latGrid && pl.lon_grid == lonGrid) {
	// save this place with it's distance
	if( nloc < maxp) {
	  pary[nloc].offset = offs0;
	  pary[nloc].distance = distance_miles( lat, lon, pl.lat, pl.lon);
	  ++nloc;
	} else {
	  fprintf( stderr, "Location array overflow, size = %ld\n", MAXLOC);
	  exit(1);
	}
	offs0 = offset;
      } else {
	break;
      }
    }
  }
  printf("read %d locations\n", nloc);
  return nloc;
}

int main( int argc, char *argv[]) {
  a_place pl;

  fp = fopen( PLACES_FILE, "rb");
  fg = fopen( GRID_FILE, "rb");
  fi = fopen( INDEX_FILE, "rb");
  if( !fp || !fg || !fi) {
    fprintf( stderr, "Failed to open a required file\n");
    exit(1);
  }

  if( argc < 3) {
    printf("usage: simulate_gps <lat> <long>\n");
    exit(0);
  }

  printf("storage for locations:  %ld (%ld each)\n", sizeof(locs), sizeof(a_loc));

  float lat = atof( argv[1]);
  float lon = atof( argv[2]);

  // read grid file
  fgets( buff, sizeof(buff), fg);
  fgets( buff, sizeof(buff), fg);
  int ng = parse_csv_line(buff, tokn, MAXT);
  if( ng != 8) {
    fprintf( stderr, "Error processing grid data: %s\n", buff);
    exit(1);
  }

  // get grid specifications from file
  int numLat = atoi( tokn[0]);
  float latMin = atof( tokn[1]);
  float latMax = atof( tokn[2]);
  float latStep = atof( tokn[3]);
  int numLon = atoi( tokn[4]);
  float lonMin = atof( tokn[5]);
  float lonMax = atof( tokn[6]);
  float lonStep = atof( tokn[7]);

  // calculate our grid location from "GPS"
  int latGrid = (lat - latMin) / latStep;
  int lonGrid = (lon - lonMin) / lonStep;

  // calculate remainder (offset within grid cell)
  float lat_rem = lat - (latMin + (float)latGrid * latStep);
  float lon_rem = lon - (lonMin + (float)lonGrid * lonStep);

  // calculate which direction to look in neighbor cells
  int latNeighbor = lat_rem > 0.5 ? latGrid+1 : latGrid-1;
  int lonNeighbor = lon_rem > 0.5 ? lonGrid+1 : lonGrid-1;

  printf("Grid: Lat %d %f .. %f step %f \n", numLat, latMin, latMax, latStep);
  printf("Grid: Lon %d %f .. %f step %f \n", numLon, lonMin, lonMax, lonStep);
  printf("Grid for lat %f lon %f is (%d, %d)\n", lat, lon, latGrid, lonGrid);
  printf("Neighbors: %d, %d\n", latNeighbor, lonNeighbor);

  // process 4 neighboring grids
  numLoc = 0;
  printf("numLoc = %d\n", numLoc);
  numLoc += process_grid_at( lat, lon, latGrid, lonGrid, &locs[numLoc], MAXLOC-numLoc);
  printf("numLoc = %d\n", numLoc);
  numLoc += process_grid_at( lat, lon, latNeighbor, lonGrid, &locs[numLoc], MAXLOC-numLoc);
  printf("numLoc = %d\n", numLoc);
  numLoc += process_grid_at( lat, lon, latGrid, lonNeighbor, &locs[numLoc], MAXLOC-numLoc);
  printf("numLoc = %d\n", numLoc);
  numLoc += process_grid_at( lat, lon, latNeighbor, lonNeighbor, &locs[numLoc], MAXLOC-numLoc);
  printf("numLoc = %d\n", numLoc);

  printf("%d places in grid squares\n", numLoc);
  qsort( locs, numLoc, sizeof(locs[0]), cmpf);

  for( int i=0; i<numLoc; i++) {
    printf("offset = %d  dist = %f : ", locs[i].offset, locs[i].distance);
    fseek( fp, locs[i].offset, SEEK_SET);
    fgets( buff, sizeof(buff), fp);
    csv_to_place( &pl, buff);
    dump_place( &pl);
  }
}
