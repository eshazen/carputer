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

// #define DEBUG

#include "distance_miles.h"
#include "places.h"
#include "parse_csv.h"

int csv_to_place( a_place* ap, char *csv);
int place_to_csv( a_place* p, char *buffer, int buffer_size);
int free_place( a_place* p);
void dump_place( a_place* p);
int parse_csv_line(char *line, char *fields[], int max_fields);
double distance_miles(double lat1, double lon1, double lat2, double lon2);

// ---- hardwired (ugh) input file names ----
#define PLACES_FILE "places.csv"
#define GRID_FILE "grid.csv"
#define INDEX_FILE "gindex.dat"

#include <SPI.h>
#include <SD.h>

const int chipSelect = SDCARD_SS_PIN;

static char buff[80];
static char prnt[128];

static char* tokn[10];
#define MAXT (sizeof(tokn)/sizeof(tokn[0]))

File fp;
File fg;
File fi;

// structure for an in-memory location
typedef struct {
  uint32_t offset;		/* offset in places file */
  float distance;		/* distance to our GPS location */
} a_loc;

// array of up to nearby locations from our grid cell
// and neighboring ones
static a_loc sortedLocs[10];
#define MAXLOC (sizeof(sortedLocs)/sizeof(a_loc))
static int numLoc;		/* pointer to last occupied in sortedLocs */

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

// debug:  dump a a_loc list
void dump_list( a_loc* list, int nlist) {
#ifdef DEBUG
  snprintf( prnt, sizeof(prnt), "dump_list( size=%d)\n", nlist);
  Serial.print(prnt);
  if( nlist)
    for( int i=0; i<nlist; i++) {
      snprintf( prnt, sizeof(prnt), "  %d 0x%08x, %f\n", i, list[i].offset, list[i].distance);
      Serial.print( prnt);
    }
#endif  
}

//
// process a grid square
//
int process_grid_at( float lat, float lon, int latGrid, int lonGrid, a_loc* list, int nlist, int maxlist) {

#ifdef DEBUG
  snprintf( prnt, sizeof(prnt), "process_grid_at( nlist = %d)\n", nlist);
  Serial.print(prnt);
#endif
  // check for out of range
  if( latGrid < 0 || lonGrid < 0 || lonGrid > 255 || latGrid > 255)
    return 0;

  a_place pl;
  a_loc loc;

  uint32_t posn = ((latGrid << 8) | lonGrid) * sizeof( uint32_t);
#ifdef DEBUG
  snprintf( prnt, sizeof(prnt), "posn (0x%02x, 0x%02x) = 0x%04x (%d)\n", latGrid, lonGrid, posn, posn);
  Serial.print(prnt);
#endif

  fi.seek( posn);

  uint32_t offset;

  fi.read( &offset, sizeof(offset));

#ifdef DEBUG
  snprintf( prnt, sizeof(prnt),  "Retrieved offset 0x%04x from position %d\n", offset, posn);
  Serial.print(prnt);
#endif
  if( offset != 0xffffff) {

#ifdef DEBUG
    snprintf( prnt, sizeof(prnt), "Seeking to offset\n");
    Serial.print(prnt);
#endif

    fp.seek( offset);
    uint32_t offs0 = offset;
    int insPt;

    // read until EOF or grid changes
    
    while( int n = fp.readBytesUntil( 0xa, buff, sizeof(buff))) {
      buff[n] = '\0';
#ifdef DEBUG
      Serial.print("Read: ");
      Serial.println( buff);
#endif      
      offset = fp.position();
      if( csv_to_place( &pl, buff)) {
	snprintf( prnt, sizeof(prnt), "Error parsing %s\n", buff);
	Serial.print(prnt);
	exit(1);
      }
      if( pl.lat_grid == latGrid && pl.lon_grid == lonGrid) {
	loc.offset = offs0;
	loc.distance = distance_miles( lat, lon, pl.lat, pl.lon);

#ifdef DEBUG
	snprintf( prnt, sizeof(prnt), "--> Processing new loc 0x%08x, %f\n", loc.offset, loc.distance);
	Serial.print(prnt);
#endif

//	// just insert unsorted for now
//	if( nlist < maxlist)
//	  list[nlist++] = loc;

	// insert in list maintaining order
#ifdef DEBUG
	snprintf( prnt, sizeof(prnt), "Before sort:");  dump_list( list, nlist);
	Serial.print(prnt);
#endif
	if( nlist == 0) {		/* empty list case */
#ifdef DEBUG
	  snprintf( prnt, sizeof(prnt), "Empty list, insert here\n");
	  Serial.print(prnt);
#endif
	  list[nlist++] = loc;
	} else {
#ifdef DEBUG
	  snprintf( prnt, sizeof(prnt), "Searching for fit...\n");
	  Serial.print(prnt);
#endif
	  for( insPt=0; insPt<nlist; insPt++) { /* else find where it fits */
#ifdef DEBUG
	    snprintf( prnt, sizeof(prnt), "Check at %d\n", insPt);
	    Serial.print(prnt);
#endif
	    if( loc.distance <= list[insPt].distance) { /* insert here */
#ifdef DEBUG
	      snprintf( prnt, sizeof(prnt), "Shifting...\n");
	      Serial.print(prnt);
#endif
	      // shift all including list[i] to right
	      for( int j=maxlist-2; j>=insPt; j--)
		list[j+1] = list[j];
	      // insert item
	      list[insPt] = loc;
	      if( nlist < maxlist)
		++nlist;
	      break;
	    }
	  }
	  if( insPt == nlist) {
#ifdef DEBUG
	    snprintf( prnt, sizeof(prnt), "At end of list\n");
	    Serial.print(prnt);
#endif
	    if( nlist < maxlist) {
#ifdef DEBUG
	      snprintf( prnt, sizeof(prnt), "Insert at loc %d\n", nlist);
	      Serial.print(prnt);
#endif
	      list[nlist++] = loc;
	    }
	  }
	}
#ifdef DEBUG
	snprintf( prnt, sizeof(prnt), "After insert:"); dump_list( list, nlist);
	Serial.print(prnt);
#endif
	offs0 = offset;
      } else {
	break;
      }
    }
  }
#ifdef DEBUG
  snprintf( prnt, sizeof(prnt), "read %d locations\n", nlist);
  Serial.print(prnt);
#endif
  return nlist;
}


static int i, j, k, n;


void loop() {
}

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");

  a_place pl;

  fp = SD.open( PLACES_FILE);
  fg = SD.open( GRID_FILE);
  fi = SD.open( INDEX_FILE);
  
  if( !fp || !fg || !fi) {
    Serial.println("Failed to open a required file\n");
    while(1) ;
  }

  // <FIXME>
  float lat = 42.3423;
  float lon = -71.1378;

  // read grid file

  n = fg.readBytesUntil( 0xa, buff, sizeof(buff));
  n = fg.readBytesUntil( 0xa, buff, sizeof(buff));
  buff[n] = '\0';
  
  int ng = parse_csv_line(buff, tokn, MAXT);
  if( ng != 8) {
    Serial.print( "Error processing grid data:");
    Serial.println( buff);
    while(1) ;
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

  snprintf( prnt, sizeof(prnt), "Grid: Lat %d %f .. %f step %f \n", numLat, latMin, latMax, latStep);
  Serial.print(prnt);
  snprintf( prnt, sizeof(prnt), "Grid: Lon %d %f .. %f step %f \n", numLon, lonMin, lonMax, lonStep);
  Serial.print(prnt);
  snprintf( prnt, sizeof(prnt), "Grid for lat %f lon %f is (%d, %d)\n", lat, lon, latGrid, lonGrid);
  Serial.print(prnt);
  snprintf( prnt, sizeof(prnt), "Neighbors: %d, %d\n", latNeighbor, lonNeighbor);
  Serial.print(prnt);

  // process 4 neighboring grids
  numLoc = 0;
  numLoc = process_grid_at( lat, lon, latGrid, lonGrid, sortedLocs, numLoc, MAXLOC);
  numLoc = process_grid_at( lat, lon, latNeighbor, lonGrid, sortedLocs, numLoc, MAXLOC);
  numLoc = process_grid_at( lat, lon, latGrid, lonNeighbor, sortedLocs, numLoc, MAXLOC);
  numLoc = process_grid_at( lat, lon, latNeighbor, lonNeighbor, sortedLocs, numLoc, MAXLOC);

  for( int i=0; i<numLoc; i++) {
    snprintf( prnt, sizeof(prnt), "offset = %d  dist = %f : ", sortedLocs[i].offset, sortedLocs[i].distance);
    Serial.print( prnt);
    fp.seek( sortedLocs[i].offset);
    //    fseek( fp, sortedLocs[i].offset, SEEK_SET);
    n = fp.readBytesUntil( 0xa, buff, sizeof(buff));
    buff[n] = '\0';
    csv_to_place( &pl, buff);
    dump_place( &pl);
  }
}
