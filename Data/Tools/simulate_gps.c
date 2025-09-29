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

static char buff[100];
static char* tokn[10];
#define MAXT (sizeof(tokn)/sizeof(tokn[0]))

a_place pl;

static uint32_t poffs[100];
#define MAXPOFF (sizeof(poffs)/sizeof(uint32_t))
static int noff = 0;

int main( int argc, char *argv[]) {
  FILE *fp = fopen( PLACES_FILE, "rb");
  FILE *fg = fopen( GRID_FILE, "rb");
  FILE *fi = fopen( INDEX_FILE, "rb");
  if( !fp || !fg || !fi) {
    fprintf( stderr, "Failed to open a required file\n");
    exit(1);
  }

  if( argc < 3) {
    printf("usage: simulate_gps <lat> <long>\n");
    exit(0);
  }

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

  int numLat = atoi( tokn[0]);
  float latMin = atof( tokn[1]);
  float latMax = atof( tokn[2]);
  float latStep = atof( tokn[3]);
  int numLon = atoi( tokn[4]);
  float lonMin = atof( tokn[5]);
  float lonMax = atof( tokn[6]);
  float lonStep = atof( tokn[7]);

  int latGrid = (int)((lat - latMin) / latStep);
  int lonGrid = (int)((lon - lonMin) / lonStep);

  printf("Grid: Lat %d %f .. %f step %f \n", numLat, latMin, latMax, latStep);
  printf("Grid: Lon %d %f .. %f step %f \n", numLon, lonMin, lonMax, lonStep);
  printf("Grid for lat %f lon %f is (%d, %d)\n", lat, lon, latGrid, lonGrid);

  // gridIndex[tp.lat_grid][tp.lon_grid] = posn;
  uint32_t posn = ((latGrid << 8) | lonGrid) * sizeof( uint32_t);
  printf("posn (0x%02x, 0x%02x) = 0x%04x (%d)\n", latGrid, lonGrid, posn, posn);
  fseek( fi, posn, SEEK_SET);

  uint32_t offset;
  fread( &offset, 1, sizeof(offset), fi);
  printf( "Retrieved offset 0x%04x from position %d\n", offset, posn);

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
      if( noff < MAXPOFF)
	poffs[noff++] = offs0;
      offs0 = offset;
    } else {
      break;
    }
  }

  printf("%d places in grid square\n", noff);
  for( int i=0; i<noff; i++) {
    fseek( fp, poffs[i], SEEK_SET);
    fgets( buff, sizeof(buff), fp);
    csv_to_place( &pl, buff);
    dump_place( &pl);
    float dist = distance_miles( lat, lon, pl.lat, pl.lon);
    printf("dist = %f mi\n", dist);
  }
}
