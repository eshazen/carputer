//
// Working display code
// Look for GPS messages
// Looup Lat/Lon using hardwired file names STUFF.DAT .VRT .PRT .REE
// Display 3 priorities (norm. City, County, State) on display
//

// #define USE_SERIAL

#include <math.h>
#include <SD.h>
#include <string.h>
#include <avr/dtostrf.h>

#include "oled.h"
#include "shape.h"
#include "filetree.h"

#define USE_GPS
// #define RAND_POS

#define USE_OLED

// #define DEBUG
// #define USE_SERIAL

int point_in_part_file( File fv, int n, coord_t x, coord_t y);
void search_tree( coord_t lat, coord_t lon, long offset, File ft, File fd, File fv, File fa);

void oled_print( int line, const char *str) {
  if( line < USE_LINES) {
    draw_text( oledBuf, str, 1, (line+1) * LINE_SPC, 15);
    send_buffer_to_OLED( oledBuf, 0, 0);
  }
}

const int chipSelect = SDCARD_SS_PIN;

char prnt[100];
char slat[12];
char slon[12];

File ft;
File fd;
File fv;
File fa;

char gpsStatus = 'X';			// last GGA message status
int gpsNumSat = 0;			// last GGA messag number of sats

long start;

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
#ifdef USE_SERIAL
	Serial.println( gps);
#endif
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

#ifdef USE_SERIAL
	if( strlen(gps) < 25 || strlen(lat) < 3 )
	  Serial.println( gps);
#endif

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




void setup() {

#ifdef USE_SERIAL
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
#endif
  
#ifdef USE_GPS  
  Serial1.begin(9600);
  flush();
#endif

#ifdef USE_SERIAL
  Serial.print("Initializing SD card...");
#endif
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
#ifdef USE_SERIAL
    Serial.println("Card failed, or not present");
#endif
    while (1);
  }
#ifdef USE_SERIAL
  Serial.println("card initialized.");
#endif

  ft = SD.open("STUFF.REE");
  fd = SD.open("STUFF.DAT");
  fv = SD.open("STUFF.VRT");
  fa = SD.open("STUFF.PRT");

  SSD1322_HW_Init();
  SSD1322_API_init();
  select_font( &FreeMono9pt7b);

  pinMode( LED_BUILTIN, OUTPUT);

  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 0, "ShapeDisplay 1.0");
  delay(1000);

  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 0, "Initializing...");
  delay(1000);

  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 0, "Ready...");
  delay(1000);

} // setup()

void loop() {

  coord_t lat, lon;
  char *gpsTime;
  float floatLat, floatLon;


#ifdef USE_GPS
  gpsTime = GPS_fix( &floatLat, &floatLon);
#endif

#ifdef RAND_POS

  const float latMin = 42.03;
  const float latMax = 42.67;

  const float lonMin = -71.78;
  const float lonMax = -70.890;

  const float latRange = (latMax-latMin);
  const float lonRange = (lonMax-lonMin);

  long start;

  floatLat = latMin + (float)(random(latRange*10000.0)/10000.0);
  floatLon = lonMin + (float)(random(lonRange*10000.0)/10000.0);
#endif  

  fill_buffer( oledBuf, 0);		// clear screen

  // check GPS status
  if( gpsTime == NULL) { 		// no GPS at all
#ifdef DEBUG
    Serial.println("NO GPS");
#endif
#ifdef USE_OLED
  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 0, "NO GPS");
#endif    
  } else {			// have at least time

    if( floatLat < 0) {
      // use a fake location to test the SW
      floatLat = 24.555;
      floatLon = -81.7840;
    }

// #ifdef DEBUG
//     Serial.println("NO LOC");
// #endif
// #ifdef USE_OLED
//   oled_print( 0, "NO LOC");
//   oled_print( USE_LINES-1, gpsTime);
// #endif    
//   } else {
  
    // display lat/lon on display
    memset( prnt, ' ', sizeof(prnt));
    dtostrf( floatLat, 7, 2, slat);
    dtostrf( floatLon, 7, 2, slon);
    gpsTime[4] = '\0';
    //    gpsTime[6] = '\0';
    // snprintf( prnt, 23, "%s %s %s %d", slat, slon, gpsTime, gpsNumSat);
    snprintf( prnt, 23, "%s %s %s %d", slat, slon, gpsTime, gpsNumSat);    
    //    snprintf( prnt, 23, "*%s*", gpsTime);
    oled_print( 3, prnt); 

#ifdef USE_SERIAL
    Serial.println("Search tree");
    start = millis();
#endif
    // search_tree will print on first 3 lines of display using database prio
    search_tree( floatLat, floatLon, 0L, ft, fd, fv, fa);

    long elapsed = millis() - start;
#ifdef USE_SERIAL
    Serial.print("ms: ");
    Serial.println( elapsed);
#endif
  }
  delay(5000);

} // loop()





#define MSIZ 0.05

void search_tree( coord_t lat, coord_t lon, long offset, File ft, File fd, File fv, File fa) {

  struct f_node fnode;
  int rc;
  f_shape fshape;
  a_point pt;

  ft.seek( offset);
  ft.read( &fnode, sizeof(fnode));
  //  rc = fread( &fnode, sizeof(fnode), 1, ft);
  //  Serial.print("Count ");
  //  Serial.println( fnode.count);

  // check against rects
  for( int i=0; i<fnode.count; i++) {
    //    Serial.print("Node ");
    //    Serial.println( i);
    if( lat >= fnode.rects[i].min[0] && lat <= fnode.rects[i].max[0] &&
	lon >= fnode.rects[i].min[1] && lon <= fnode.rects[i].max[1]) {
      if( fnode.kind == LEAF) { /* LEAF */
	fd.seek( fnode.item_offsets[i]);
	// fread( &fshape, sizeof(fshape), 1, fd);
	fd.read( &fshape, sizeof(fshape));
//	Serial.print("LEAF ");
//	Serial.println( fshape.name);
	// now point to the virtex list
	// fseek( fv, fshape.points_off, SEEK_SET);
	fv.seek( fshape.points_off);
	// point to parts
	fa.seek( fshape.part_off);

	for( int j=0; j<fshape.nparts; j++) {
	  a_part prt;
	  // fread( &prt, sizeof(prt), 1, fa);
	  fa.read( &prt, sizeof(prt));
	  long so = prt.offset*sizeof(a_point)+fshape.points_off;
	  fv.seek( so);

	  if( point_in_part_file( fv, prt.count, lon, lat )) {
	    snprintf( prnt, sizeof(prnt), "%s", fshape.name);
#ifdef USE_SERIAL
	    Serial.println( prnt);
#endif
	    oled_print( 2-fshape.prio, prnt);
	  }
	}
      } else {		  /* BRANCH */
	search_tree( lat, lon, fnode.node_offsets[i]*sizeof(fnode), ft, fd, fv, fa);
      }
    }
  }
}



