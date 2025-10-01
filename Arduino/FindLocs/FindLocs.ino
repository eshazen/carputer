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


#define DEBUG
#define USE_OLED
#define USE_GPS

#define MAX_LOCS 3

#include <math.h>
#include <SPI.h>
#include <SD.h>
#include <string.h>

#ifdef USE_OLED
#include "SSD1322_HW_Driver.h"
#include "SSD1322_API.h"
#include "SSD1322_GFX.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/vga9x167pt7b.h"
#define LINE_SPC 14
uint8_t oledBuf[OLED_HEIGHT*OLED_WIDTH];
#endif

#include "distance_miles.h"
#include "places.h"
#include "parse_csv.h"

// int csv_to_place( a_place* ap, char *csv);
// int place_to_csv( a_place* p, char *buffer, int buffer_size);
// int free_place( a_place* p);
// void dump_place( a_place* p);
// int parse_csv_line(char *line, char *fields[], int max_fields);
// double distance_miles(double lat1, double lon1, double lat2, double lon2);

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

// structure for an in-memory location
typedef struct {
  uint32_t offset;		/* offset in places file */
  float distance;		/* distance to our GPS location */
} a_loc;

// array of up to nearby locations from our grid cell
// and neighboring ones
a_loc sortedLocs[3];
#define MAXLOC (sizeof(sortedLocs)/sizeof(a_loc))
static int numLoc;		/* pointer to last occupied in sortedLocs */



#ifdef USE_GPS
//
// try to get a GPS fix
// return 1 on error, 0 on success
//
// blocks until receive a GPS message $GPRMC on Serial1
//
int GPS_fix( float *flat, float *flon) {

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

  while( 1) {

    if( Serial1.available()) {
      int nc = Serial1.readBytesUntil( '\n', gps, sizeof(gps)-1);
      gps[nc] = '\0';
      if( !strncasecmp( gps, "$GPRMC", 6)) {
	Serial.println( gps);
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

	if( strlen(lat) < 5 || strlen(lon) < 5) {	// waiting for fix?
	
	  snprintf( prnt, sizeof(prnt), "%s %s", stat, gmt);
#ifdef DEBUG
	  Serial.println( prnt);
#endif
#ifdef USE_OLED	
	  oled_print( 0, (char *)oledBuf);
#endif
	  return 1;		// got a message but no fix yet
	} else {			// got a fix

	  // convert lat/lon
	  *flat = conv_ll( lat);
	  if( toupper(*ns) == 'S')
	    *flat = -*flat;
	  *flon = conv_ll( lon);
	  if( toupper(*ew) == 'W')
	    *flon = -*flon;
	  return 0;

	} // if( got a fix)
      }
    }
  } // while(1)
}

// convert NEMA lat/long to decimal degrees
// format is:  dddmm.ffff
//    ddd is integer degrees (1-3 digits)
//     mm is integer minutes
//   ffff is fractional minutes
float conv_ll( char *str) {
  float minu;
  float degr;
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



#ifdef USE_OLED
void oled_print( int line, char *str) {
  draw_text( oledBuf, str, 1, (line % 4) * LINE_SPC, 15);
  send_buffer_to_OLED( oledBuf, 0, 0);
}
#endif


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

  // why ???
  a_loc *list;
  list = (a_loc *)vlist;

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
#ifdef DEBUG
	snprintf( prnt, sizeof(prnt), "Error parsing %s\n", buff);
	Serial.print(prnt);
#endif
	while(1) ;
      }
      if( pl.lat_grid == latGrid && pl.lon_grid == lonGrid) {
	loc.offset = offs0;
	loc.distance = distance_miles( lat, lon, pl.lat, pl.lon);

#ifdef DEBUG
	snprintf( prnt, sizeof(prnt), "--> Processing new loc 0x%08x, %f\n", loc.offset, loc.distance);
	Serial.print(prnt);
#endif

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


void setup() {

  pinMode( LED_BUILTIN, OUTPUT);
#ifdef USE_OLED
  SSD1322_HW_Init();
  SSD1322_API_init();
  select_font( &FreeSans9pt7b);
  //  select_font( &vga_9x167pt7b);
#endif

#ifdef USE_GPS  
  Serial1.begin(9600);
  flush();
#endif

#ifdef DEBUG
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.print("Initializing SD card...");
#endif

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {

#ifdef USE_OLED
    oled_print( 0, "uSD FAIL");
#endif    
#ifdef DEBUG
    Serial.println("Card failed, or not present");
#endif
    while (1);
  }
#ifdef USE_OLED
  oled_print( 0, "uSD OK");
#endif
#ifdef DEBUG
  Serial.println("card initialized.");
#endif
  
  a_place pl;

  fp = SD.open( PLACES_FILE);
  fg = SD.open( GRID_FILE);
  fi = SD.open( INDEX_FILE);
  
  if( !fp || !fg || !fi) {
#ifdef USE_OLED
    oled_print( 0, "File missing");
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
    oled_print( 0, "Grid error");
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


void loop() {
  float floatLat;
  float floatLon;
  a_place pl;

#ifdef USE_OLED
  int pos = LINE_SPC;
  fill_buffer( oledBuf, 0);		// clear screen
#endif

#ifdef USE_GPS
  int gpsStatus = GPS_fix( &floatLat, &floatLon);
#else
  int gpsStatus = 0;
#endif  

  if( gpsStatus) { 		// no fix
#ifdef DEBUG
    Serial.println("NO GPS");
#endif
#ifdef USE_OLED
    oled_print( 0, "NO GPS");
#endif    

  } else {			// got a fix!

#ifndef USE_GPS
    floatLat = 42.1111;
    floatLon = -72.3333;
#endif    

    // calculate our grid location from "GPS"
    int latGrid = (floatLat - latMin) / latStep;
    int lonGrid = (floatLon - lonMin) / lonStep;

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
    numLoc = process_grid_at( floatLat, floatLon, latNeighbor, lonGrid, sortedLocs, numLoc, MAXLOC);
    numLoc = process_grid_at( floatLat, floatLon, latGrid, lonNeighbor, sortedLocs, numLoc, MAXLOC);
    numLoc = process_grid_at( floatLat, floatLon, latNeighbor, lonNeighbor, sortedLocs, numLoc, MAXLOC);

    for( int i=0; i<numLoc; i++) {
#ifdef DEBUG
      snprintf( prnt, sizeof(prnt), "offset = %d  dist = %f : ", sortedLocs[i].offset, sortedLocs[i].distance);
      Serial.print( prnt);
#endif
      fp.seek( sortedLocs[i].offset);
      //    fseek( fp, sortedLocs[i].offset, SEEK_SET);
      n = fp.readBytesUntil( 0xa, buff, sizeof(buff));
      buff[n] = '\0';
      csv_to_place( &pl, buff);
#ifdef DEBUG
      dump_place( &pl);
#endif
#ifdef USE_OLED
      place_to_short( &pl, buff, 20);
#endif    
    } // loop over nearest places

  } // else got a fix

} // loop()




