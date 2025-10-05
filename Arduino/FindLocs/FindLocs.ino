//
// First version display code
//
// read GPS lat/long
// display GMT time and nearest places
//
// use files:
//   places.csv    - places annotated with grid coordinates
//   grid.csv      - one line of grid spacing/range info
//   gindex.dat    - 256x256 array of 32-bit binary integers
//                   with offsets to start of each grid cell


//#define DEBUG
#define USE_OLED
#define USE_GPS

// #define NO_INSERT
#define DO_INSERT

#include <math.h>
#include <SPI.h>
#include <SD.h>
#include <string.h>

#ifdef USE_OLED
#include "SSD1322_HW_Driver.h"
#include "SSD1322_API.h"
#include "SSD1322_GFX.h"
#include "Fonts/FreeMono9pt7b.h"
#include "Fonts/FreeMono12pt7b.h"
#include "Fonts/FreeMono18pt7b.h"

#define LINE_SPC 14
#define USE_LINES 4
uint8_t oledBuf[OLED_HEIGHT*OLED_WIDTH];
#endif

#include "distance_miles.h"
#include "places.h"
#include "parse_csv.h"

// ---- hardwired (ugh) input file names ----
#define PLACES_FILE "places.csv"
#define GRID_FILE "grid.csv"
#define INDEX_FILE "gindex.dat"

const int chipSelect = SDCARD_SS_PIN;

static char buff[80];
static char prnt[128];

static char* tokn[10];
#define MAXT (sizeof(tokn)/sizeof(tokn[0]))

File fp;
File fg;
File fi;

// grid specifications from file
int numLat;
float latMin;
float latMax;
float latStep;
int numLon;
float lonMin;
float lonMax;
float lonStep;

char gpsStatus = 'X';			// last GGA message status
int gpsNumSat = 0;			// last GGA messag number of sats

static a_place tempPlace;

// structure for an in-memory location
typedef struct {
  uint32_t offset;		/* offset in places file */
  float distance;		/* distance to our GPS location */
} a_loc;

// array of up to nearby locations from our grid cell
// and neighboring ones
a_loc sortedLocs[10];
#define MAXLOC (sizeof(sortedLocs)/sizeof(a_loc))
static int numLoc;		/* pointer to last occupied in sortedLocs */

#ifdef USE_OLED
void oled_print( int line, const char *str) {
  if( line < USE_LINES) {
    draw_text( oledBuf, str, 1, (line+1) * LINE_SPC, 15);
    send_buffer_to_OLED( oledBuf, 0, 0);
  }
}
#endif



#ifdef USE_GPS
//
// try to get a GPS fix
// return pointer to GMT time on success, NULL on error
// if location not available, set lat = lon = -1
//
// blocks until receive a GPS message $GPRMC on Serial1
//
char* GPS_fix( float *flat, float *flon) {

  static char tyme[40];

  char gps[80];
  char *str;
  char *p;
  char *gmt;
  char *date;
  char *stat;
  char *lat;
  char *ns;
  char *lon;
  char *ew;

  char *ret = NULL;

  while( 1) {

    if( Serial1.available()) {
      int nc = Serial1.readBytesUntil( '\n', gps, sizeof(gps)-1);
      gps[nc] = '\0';

      if( !strncasecmp( gps, "$GPGGA", 6)) {

	str = gps;
	Serial.println( gps);

	p = strsep( &str, ",");	// 0 skip msg
	p = strsep( &str, ",");	// 1 skip time
	p = strsep( &str, ",");	// 2 skip lat
	p = strsep( &str, ",");	// 3 skip ns
	p = strsep( &str, ",");	// 4 skip lon
	p = strsep( &str, ",");	// 5 skip ew
	ew = strsep( &str, ",");// 6 position fix status
	ns = strsep( &str, ",");// 7 num sats

	gpsStatus = ew[0];
	gpsNumSat = atoi( ns);
      }

      if( !strncasecmp( gps, "$GPRMC", 6)) {
	// parse and display stuff
	str = gps;
	p = strsep( &str, ",");	// skip over header   0
	gmt = strsep( &str, ",");	// get UTC time       1
	stat = strsep( &str, ","); // get status        2
	lat = strsep( &str, ","); // get latitude       3
	ns = strsep( &str, ","); // n/s                 4
	lon = strsep( &str, ","); // get longitude      5
	ew = strsep( &str, ","); // e/w                 6
	p = strsep( &str, ","); // skip                 7
	p = strsep( &str, ","); // skip                 8
	date = strsep( &str, ","); // date              9

	if( strlen(gps) < 25 || strlen(lat) < 3 )
	  Serial.println( gps);

	if( strlen(gmt) > 3) {
	  strncpy( tyme, gmt, sizeof(tyme));
	  ret = tyme;
	}

	*flat = *flon = -1.0;	// default return values

	  if( strlen(lat) > 5 && strlen(lon) > 5) {	// got a fix

	  // convert lat/lon
	  *flat = conv_ll( lat);
	  if( toupper(*ns) == 'S')
	    *flat = -*flat;
	  *flon = conv_ll( lon);
	  if( toupper(*ew) == 'W')
	    *flon = -*flon;
	} // if( got a fix)

	return ret;

      } // if( $GPRMC)
    }
  } // while(1)
}

// convert NEMA lat/long to decimal degrees
// format is:  dddmm.ffff
//    ddd is integer degrees (1-3 digits)
//     mm is integer minutes
//   ffff is fractional minutes
float conv_ll( char *str) {
  static float minu;
  static float degr;
  char *dp = strchr( str, '.');  // point to decimal
  if( dp == NULL)
    return( 0.0);
  dp -= 2;              // point to minutes
  minu = atof(dp);      // convert floating minutes
  *dp = '\0';           // terminate degrees
  degr = atof( str);    // convert float degrees
  degr += minu / 60.0;  // add minutes
  return degr;          // return result
}

#endif

void flush() {
#ifdef USE_GPS
  while( Serial1.available())
    Serial1.read();
#endif
}



#ifdef DEBUG
// debug:  dump a a_loc list
void dump_list( a_loc *list, int nlist) {
  snprintf( prnt, sizeof(prnt), "dump_list( size=%d)\n", nlist);
  Serial.print(prnt);
  if( nlist) {
    for( int i=0; i<nlist; i++) {
      snprintf( prnt, sizeof(prnt), "  %d 0x%08x, %f\n", i, list[i].offset, list[i].distance);
      Serial.print( prnt);
    }
  }
}
#endif  

//
// process a grid square
//
//int process_grid_at( float lat, float lon, int latGrid, int lonGrid, a_loc* list, int nlist, int maxlist) {
int process_grid_at( float lat, float lon, int latGrid, int lonGrid, void* vlist, int nlist, int maxlist) {

#ifdef DEBUG
  snprintf( prnt, sizeof(prnt), "pg() %f %f %d %d 0x%x %d %d\n",
	    lat, lon, latGrid, lonGrid, (int)vlist, nlist, maxlist);
  Serial.print( prnt);
#endif  

  // why ??? GCC / Arduino bug prevents using a_loc* in arg list
  a_loc *list;
  list = (a_loc *)vlist;

  uint32_t offset;
  a_loc loc;
  int numThisGrid = 0;

  // check for out of range
  if( latGrid < 0 || lonGrid < 0 || lonGrid > 255 || latGrid > 255)
    return 0;

  uint32_t posn = ((latGrid << 8) | lonGrid) * sizeof( uint32_t);
  fi.seek( posn);
  fi.read( &offset, sizeof(offset));

#ifdef DEBUG
    snprintf( prnt, sizeof(prnt), "Seek %ld\n", offset);
    Serial.print(prnt);
#endif

  if( offset != 0xffffffff) {

    fp.seek( offset);
    uint32_t offs0 = offset;
    int insPt;

#ifdef DEBUG
    snprintf( prnt, sizeof(prnt), "Seeked %ld\n", offset);
    Serial.print(prnt);
#endif

    // read until EOF or grid changes
    
    while( int n = fp.readBytesUntil( 0xa, buff, sizeof(buff))) {
      buff[n] = '\0';
      offset = fp.position();
      if( csv_to_place( &tempPlace, buff)) {
	oled_print(0,"FAIL ON CSV");
	while(1) ;
      }

#ifdef DEBUG
      snprintf( prnt, sizeof(prnt), "Grid %d, %d\n", tempPlace.lat_grid, tempPlace.lon_grid);
      Serial.print( prnt);
#endif

      if( tempPlace.lat_grid == latGrid && tempPlace.lon_grid == lonGrid) {
	++numThisGrid;
	loc.offset = offs0;
	loc.distance = distance_miles( lat, lon, tempPlace.lat, tempPlace.lon);

#ifdef NO_INSERT
	// for debug, just fill the list
	if( nlist < maxlist)
	  list[nlist++] = loc;
#endif

#ifdef DO_INSERT
	// insert in list maintaining order
	if( nlist == 0) {		/* empty list case */
	  list[nlist++] = loc;
	} else { // list not empty, occupied from 0..nlist-1

	  for( insPt=0; insPt<nlist; insPt++) { /* else find where it fits */
	    if( loc.distance <= list[insPt].distance) { /* insert here */
	      // check for end
	      if( insPt != maxlist-1) { // only if not at end
		// shift all including list[insPt] to right
		for( int j=maxlist-2; j>=insPt; j--)
		  list[j+1] = list[j];
	      }
	      // insert item
	      list[insPt] = loc;
	      if( nlist < maxlist-1)
		++nlist;
	      break;
	    }
	  }
	}
#endif
	offs0 = offset;
      } else {			// if( same grid)
	break;			// break loop if not
      }
    }
  }
#ifdef DEBUG
  snprintf( prnt, sizeof(prnt), "numThisGrid=%d\n", numThisGrid);
  Serial.print(prnt);
  snprintf( prnt, sizeof(prnt), "read %d locations\n", nlist);
  Serial.print(prnt);
#endif
  return nlist;
}


static int i, j, k, n;


void setup() {
#ifdef USE_OLED
  SSD1322_HW_Init();
  SSD1322_API_init();
  select_font( &FreeMono9pt7b);
#endif
#ifdef DEBUG
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.print("Initializing SD card...");
#endif

#ifdef USE_GPS  
  Serial1.begin(9600);
  flush();
#endif

  pinMode( LED_BUILTIN, OUTPUT);
#ifdef USE_OLED
  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 0, "FindLocs 1.0");
  delay(1000);
#endif

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {

#ifdef USE_OLED
    fill_buffer( oledBuf, 0);		// clear screen
    oled_print( 1, "uSD FAIL");
#endif    
#ifdef DEBUG
    Serial.println("Card failed, or not present");
#endif
    while (1);
  }
#ifdef USE_OLED
  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 0, "uSD OK");
#endif
#ifdef DEBUG
  Serial.println("card initialized.");
#endif
  
  fp = SD.open( PLACES_FILE);
  fg = SD.open( GRID_FILE);
  fi = SD.open( INDEX_FILE);
  
  if( !fp || !fg || !fi) {
#ifdef USE_OLED
  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 1, "File missing");
#endif    
#ifdef DEBUG
    Serial.println("Failed to open a required file\n");
#endif
    while(1) ;
  }

  // read grid file

  n = fg.readBytesUntil( 0xa, buff, sizeof(buff));
  n = fg.readBytesUntil( 0xa, buff, sizeof(buff));
  buff[n] = '\0';
  
  int ng = parse_csv_line(buff, tokn, MAXT);
  if( ng != 8) {
#ifdef USE_OLED
    fill_buffer( oledBuf, 0);		// clear screen
    oled_print( 1, "Grid error");
#endif    
#ifdef DEBUG
    Serial.print( "Error processing grid data:");
    Serial.println( buff);
#endif
    while(1) ;
  }

  // get grid specifications from file
   numLat = atoi( tokn[0]);
   latMin = atof( tokn[1]);
   latMax = atof( tokn[2]);
   latStep = atof( tokn[3]);
   numLon = atoi( tokn[4]);
   lonMin = atof( tokn[5]);
   lonMax = atof( tokn[6]);
   lonStep = atof( tokn[7]);
}

int fake_time = 0;
#ifndef USE_GPS
char gpsFake[32];
#endif

void loop() {

  float floatLat;
  float floatLon;

  int update_time = 0;
  const char *gpsTime;

#ifdef USE_GPS
  gpsTime = GPS_fix( &floatLat, &floatLon);
#else
  floatLat = 42.0 + (float)random(100)/100;
  floatLon = -71.8 + (float)random(100)/100;
  fake_time++;
  snprintf( gpsFake, sizeof(gpsFake), "T%d %5.1f %5.1f", fake_time, floatLat, floatLon);
  gpsTime = gpsFake;
  delay(1000);
#endif  

  // check GPS status
  if( gpsTime == NULL) { 		// no GPS at all
#ifdef DEBUG
    fill_buffer( oledBuf, 0);		// clear screen
    Serial.println("NO GPS");
#endif
#ifdef USE_OLED
  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 0, "NO GPS");
#endif    
  } else if( floatLat < 0) {	// no location
#ifdef DEBUG
    fill_buffer( oledBuf, 0);		// clear screen
    Serial.println("NO LOC");
#endif
#ifdef USE_OLED
  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 0, "NO LOC");
  oled_print( USE_LINES-1, gpsTime);
#endif    
  } else {

//#ifndef USE_GPS
//    floatLat = 42.1111;
//    floatLon = -72.3333;
//#endif    

    // calculate our grid location from "GPS"
    int latGrid = (floatLat - latMin) / latStep;
    int lonGrid = (floatLon - lonMin) / lonStep;

    int oldLatGrid, oldLonGrid;

    // calculate remainder (offset within grid cell)
    float lat_rem = floatLat - (latMin + (float)latGrid * latStep);
    float lon_rem = floatLon - (lonMin + (float)lonGrid * lonStep);

    // calculate which direction to look in neighbor cells
    int latNeighbor = lat_rem > 0.5 ? latGrid+1 : latGrid-1;
    int lonNeighbor = lon_rem > 0.5 ? lonGrid+1 : lonGrid-1;

#ifdef DEBUG
    snprintf( prnt, sizeof(prnt), "Grid: Lat %d %f .. %f step %f \n", numLat, latMin, latMax, latStep);
    Serial.print(prnt);
    snprintf( prnt, sizeof(prnt), "Grid: Lon %d %f .. %f step %f \n", numLon, lonMin, lonMax, lonStep);
    Serial.print(prnt);
    snprintf( prnt, sizeof(prnt), "Grid for lat %f lon %f is (%d, %d)\n", floatLat, floatLon, latGrid, lonGrid);
    Serial.print(prnt);
    snprintf( prnt, sizeof(prnt), "Neighbors: %d, %d\n", latNeighbor, lonNeighbor);
    Serial.print(prnt);
#endif
  
    // process 4 neighboring grids
    numLoc = 0;
    numLoc = process_grid_at( floatLat, floatLon, latGrid, lonGrid, sortedLocs, numLoc, MAXLOC);
#ifdef DEBUG
    snprintf( prnt, sizeof(prnt), "process.. returned %d\n", numLoc);
    Serial.print(prnt);
#endif    

    numLoc = process_grid_at( floatLat, floatLon, latNeighbor, lonGrid, sortedLocs, numLoc, MAXLOC);
    numLoc = process_grid_at( floatLat, floatLon, latGrid, lonNeighbor, sortedLocs, numLoc, MAXLOC);
    numLoc = process_grid_at( floatLat, floatLon, latNeighbor, lonNeighbor, sortedLocs, numLoc, MAXLOC);

    // update only if we moved
    if( 1) {
      //    if( latGrid != oldLatGrid || lonGrid != oldLonGrid) {

      oldLatGrid = latGrid;
      oldLonGrid = lonGrid;
      
      fill_buffer( oledBuf, 0);		// clear screen

      for( int i=0; i<numLoc; i++) {
#ifdef DEBUGXXX
	snprintf( prnt, sizeof(prnt), "offset = %d  dist = %f : ", sortedLocs[i].offset, sortedLocs[i].distance);
	Serial.print( prnt);
#endif
	fp.seek( sortedLocs[i].offset);
	n = fp.readBytesUntil( 0xa, buff, sizeof(buff));
	buff[n] = '\0';
	csv_to_place( &tempPlace, buff);
#ifdef DEBUGXXX
	dump_place( &tempPlace);
#endif
#ifdef USE_OLED
	place_to_short( &tempPlace, buff, 40, sortedLocs[i].distance);
	if( i < 3)
	  oled_print( i, buff);
#endif    
      } // loop over nearest places
      // status line at bottom
      snprintf( buff, sizeof(buff), "%6.6s %c %d", gpsTime, gpsStatus, gpsNumSat);
      select_font( &FreeMono12pt7b);
      //    draw_text( oledBuf, str, 1, (line+1) * LINE_SPC, 15);
      draw_text( oledBuf, buff, 0, 63, 8);
      send_buffer_to_OLED( oledBuf, 0, 0);
      select_font( &FreeMono9pt7b);
    }

  } // else got a fix

} // loop()




