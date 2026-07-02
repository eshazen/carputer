//
// Starting on V3.0
//
// Working display code
// Look for GPS messages
// Looup Lat/Lon using hardwired file names STUFF.DAT .VRT .PRT .REE
// Display highest 3 priorities (norm. Neighborhood, City, County, State) on display
//
// Play "test.wav" sound for new locations
//

#include <AudioZero.h>

#include <math.h>
#include <SD.h>
#include <string.h>
#include <avr/dtostrf.h>
#include <stdint.h>
#include <stddef.h>

#include "oled.h"
#include "shape.h"
#include "filetree.h"
#include "replac.c"

#define USE_GPS
// #define RAND_POS

// lookup timezone (slow!)
#define USE_ZONE

#define USE_OLED

// #define DEBUG
// #define USE_SERIAL

// maximum priority data we can handle
#define MAX_PRIO 8
uint32_t prio_item[MAX_PRIO];	// offsets in datafile for current search priority items
uint32_t prio_hash, prio_new_hash;		// hash of last set

int point_in_part_file( File fv, int n, coord_t x, coord_t y);
void search_tree( coord_t lat, coord_t lon, long offset, File ft, File fd, File fv, File fa, int flag);
uint32_t hash_u32_list(const uint32_t *values, size_t count);

const int chipSelect = SDCARD_SS_PIN;

char prnt[100];
char slat[12];
char slon[12];
char tzon = '?';		// set to E, C, M, P
const char* zonz = "ECMP";

File ft;
File fd;
File fv;
File fa;

File zt;
File zd;
File zv;
File za;


char gpsStatus = 'X';			// last GGA message status
int gpsNumSat = 0;			// last GGA messag number of sats

long start;

//
// try to get a GPS fix
// return pointer to GMT time on success, NULL on error
// if location not available, set lat = lon = -1
// if GPS times out, return NULL
// 
// wait 5s to receive a GPS message $GPRMC on Serial1
//

// global to keep track
static char tyme[40];
char gps[80];
char *gps_str;
char *gps_p;
char *gmt;
char *date;
char *stat;
char *lat;
char *ns;
char *lon;
char *ew;
char *ret;

#ifdef USE_GPS

char* GPS_fix( float *flat, float *flon) {
  ret = NULL;
  start = millis();
  
  while( (millis() - start) < 5000) {

    if( Serial1.available()) {
      int nc = Serial1.readBytesUntil( '\n', gps, sizeof(gps)-1);
      gps[nc] = '\0';

      if( !strncasecmp( gps, "$GPGGA", 6)) {

	gps_str = gps;
#ifdef USE_SERIAL
	Serial.println( gps);
#endif
	gps_p = strsep( &gps_str, ",");	// 0 skip msg
	gps_p = strsep( &gps_str, ",");	// 1 skip time
	gps_p = strsep( &gps_str, ",");	// 2 skip lat
	gps_p = strsep( &gps_str, ",");	// 3 skip ns
	gps_p = strsep( &gps_str, ",");	// 4 skip lon
	gps_p = strsep( &gps_str, ",");	// 5 skip ew
	ew = strsep( &gps_str, ",");// 6 position fix status
	ns = strsep( &gps_str, ",");// 7 num sats

	gpsStatus = ew[0];
	gpsNumSat = atoi( ns);
      }

      if( !strncasecmp( gps, "$GPRMC", 6)) {
#ifdef USE_SERIAL
	if( strlen(gps) < 25 || strlen(lat) < 3 )
	  Serial.println( gps);
#endif

	// parse and display stuff
	gps_str = gps;
	gps_p = strsep( &gps_str, ",");	// skip over header   0
	gmt = strsep( &gps_str, ",");	// get UTC time       1
	stat = strsep( &gps_str, ","); // get status        2
	lat = strsep( &gps_str, ","); // get latitude       3
	ns = strsep( &gps_str, ","); // n/s                 4
	lon = strsep( &gps_str, ","); // get longitude      5
	ew = strsep( &gps_str, ","); // e/w                 6
	gps_p = strsep( &gps_str, ","); // skip                 7
	gps_p = strsep( &gps_str, ","); // skip                 8
	date = strsep( &gps_str, ","); // date              9 format ddmmyy

	if( strlen(gmt) > 3) {
	  strncpy( tyme, gmt, sizeof(tyme));
	  ret = tyme;
	} else {
	  strcpy( tyme, "NONE");
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
  return ret;
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

  pinMode( A0, OUTPUT);
  digitalWrite( A0, LOW);

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

  pinMode( LED_BUILTIN, OUTPUT);

  oled_init();

  oled_clear();		// clear screen
  oled_print( 0, "TownFinder(TM) 2.0");
  delay(1000);

  oled_clear();		// clear screen
  oled_print( 0, "Initializing...");
  delay(1000);

#ifdef USE_SERIAL
  Serial.print("Initializing SD card...");
#endif
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
#ifdef USE_SERIAL
    Serial.println("Card failed, or not present");
#endif
    oled_clear();		// clear screen
    oled_print( 0, "SD Fail");
    while (1)
      ;
  } else {
#ifdef USE_SERIAL
    Serial.println("card initialized.");
#endif
  }

  if( !SD.exists("STUFF.REE") ||  !SD.exists("STUFF.DAT") ||
      !SD.exists("STUFF.VRT") ||  !SD.exists("STUFF.PRT")
#ifdef USE_ZONE
      || !SD.exists("ZONE.REE") ||  !SD.exists("ZONE.DAT") ||
      !SD.exists("ZONE.VRT") ||  !SD.exists("ZONE.PRT")
#endif
      ) {
    oled_clear();		// clear screen
    oled_print( 0, "Missing File!");

    while(1)
      ;
    
  } else {
    ft = SD.open("STUFF.REE");
    fd = SD.open("STUFF.DAT");
    fv = SD.open("STUFF.VRT");
    fa = SD.open("STUFF.PRT");
#ifdef USE_ZONE
    zt = SD.open("ZONE.REE");
    zd = SD.open("ZONE.DAT");
    zv = SD.open("ZONE.VRT");
    za = SD.open("ZONE.PRT");
#endif
  }


  oled_clear();		// clear screen
  oled_print( 0, "Ready...");
  delay(1000);

} // setup()

int loop_count = 0;
char *gpsTime;
float floatLat, floatLon;

void loop() {
  ++loop_count;

#ifdef USE_GPS
  gpsTime = GPS_fix( &floatLat, &floatLon);
#endif

#ifdef RAND_POS

  const float latMin = 42.3110;
  const float latMax = 42.3833;

  const float lonMin = -71.1134;
  const float lonMax = -71.0514;

  const float latRange = (latMax-latMin);
  const float lonRange = (lonMax-lonMin);

  long start;

  floatLat = latMin + (float)(random(latRange*10000.0)/10000.0);
  floatLon = lonMin + (float)(random(lonRange*10000.0)/10000.0);
#endif  

  oled_clear();		// clear screen

  // check GPS status
  if( gpsTime == NULL) { 		// no GPS at all
#ifdef DEBUG
    Serial.println("NO GPS");
#endif
#ifdef USE_OLED
  oled_clear();		// clear screen
  oled_print( 0, "NO GPS");
#endif    
  } else {			// have at least time

    if( floatLat < 0) {
//       if( loop_count % 2) {
      // use a fake location to test the SW
//	floatLat = 42.3135;
//	floatLon = -71.0720;
//      } else {
	floatLat = 47.9086;
	floatLon = -124.6373;
//      }
    }

#ifdef USE_ZONE
    // lookup timezone first
    start = millis();
    search_tree( floatLat, floatLon, 0L, zt, zd, zv, za, 1);
#ifdef DEBUG
    Serial.print("Zone ms: ");
    Serial.println( millis() - start);
#endif    
#endif
    
    // display lat/lon on display
    memset( prnt, ' ', sizeof(prnt));
    dtostrf( floatLat, 7, 2, slat);
    dtostrf( floatLon, 7, 2, slon);
    gpsTime[4] = '\0';
#ifdef USE_ZONE
    // adjust hour for timezone
    char *tzp;
    if( (tzp = strchr( zonz, tzon)) != NULL) {
      int itim = atoi( gpsTime); // time as 4-digit value
      int iadj = (tzp-zonz+5) * 100;
#ifdef DEBUG
      Serial.println("Time");
      Serial.println( itim);
      Serial.println( iadj);
#endif
      itim -= iadj;
      if( itim < 0)
	itim += 2400;
#ifdef DEBUG
      Serial.println( itim);
#endif
      sprintf( gpsTime, "%04d", itim);
#ifdef DEBUG
      Serial.println( gpsTime);
#endif
    }
#endif
#ifdef USE_SERIAL
    Serial.println("Search tree");
    start = millis();
#endif

    // FIXME:
    // Need to update clock once a minute, but other stuff
    // only when the data changes

    // search_tree will save item offsets in prio_item[]
    memset( prio_item, 0, sizeof( prio_item));
    search_tree( floatLat, floatLon, 0L, ft, fd, fv, fa, 0);
    // now we have a sparsely-filled array of prio_item
    // display if there was a change
    prio_new_hash = hash_u32_list( prio_item, MAX_PRIO);
    if( prio_new_hash != prio_hash) {
      prio_hash = prio_new_hash;

      oled_text_clear();
      snprintf( prnt, OLED_LINE_WIDTH, "%s %s %.2s/%.2s/%.2s %.2s:%.2s %d", slat, slon, 
	      &date[2], &date[0], &date[4], &gpsTime[0], &gpsTime[2], gpsNumSat);    
      oled_text_line( USE_LINES-1, prnt);

      int text_line = 0;
      for( int i=0; i<MAX_PRIO; i++) {
	if( prio_item[i]) { 
	  f_shape fshape;
	  fd.seek( prio_item[i]);
	  fd.read( &fshape, sizeof(fshape));
	  if( text_line < LARGE_LINES) {
	    // shorten some things
	    replac( fshape.name, "county", "cty");
	    replac( fshape.name, " town", "");
	    replac( fshape.name, "city", "");
	    replac( fshape.name, "CDP", "");
	    oled_text_line( text_line, fshape.name);
	    ++text_line;
	  }
	}
      }

      // open wave file from sdcard
      File myFile = SD.open("test.wav");
      // 44100kHz stereo => 88200 sample rate
      // AudioZero.begin(2*44100);
      AudioZero.begin(11025);
      if (myFile) {
	AudioZero.play(myFile);
	AudioZero.end();
      }

    } else {			// duplicate, do nothing for now
      ;
    }
    

    oled_text_fill_up();
    // some funny business here
    // copy the corrected time to the end of the first large line
    char *lp = oled_get_text_line( 0);
    snprintf( prnt, OLED_LINE0_WIDTH, "%-15.15s %.2s:%.2s", lp, &gpsTime[0], &gpsTime[2]);
    oled_text_line( 0, prnt);

    oled_text_update();

#ifdef USE_SERIAL
    Serial.print("ms: ");
    Serial.println( millis() - start);
#endif
  }

#ifdef USE_ZONE  
  delay(100);
#else
  delay(5000);
#endif  

} // loop()



//
// search for lat/lon in location database specified by 4 files
// action when found depends on flag:
//   flag=0  store name in dpy_line[] up to OLED_LINE_WIDTH characters
//   flag=1  set the timezone
//
void search_tree( coord_t lat, coord_t lon, long offset, File ft, File fd, File fv, File fa, int flag) {

  struct f_node fnode;
  int rc;
  f_shape fshape;
  a_point pt;

  ft.seek( offset);
  ft.read( &fnode, sizeof(fnode));

  // check against rects
  for( int i=0; i<fnode.count; i++) {
    if( lat >= fnode.rects[i].min[0] && lat <= fnode.rects[i].max[0] &&
	lon >= fnode.rects[i].min[1] && lon <= fnode.rects[i].max[1]) {
      if( fnode.kind == LEAF) { /* LEAF */
	fd.seek( fnode.item_offsets[i]);
	fd.read( &fshape, sizeof(fshape));
	fv.seek( fshape.points_off);
	fa.seek( fshape.part_off);

	for( int j=0; j<fshape.nparts; j++) {
	  a_part prt;
	  fa.read( &prt, sizeof(prt));
	  long so = prt.offset*sizeof(a_point)+fshape.points_off;
	  fv.seek( so);

	  if( point_in_part_file( fv, prt.count, lon, lat )) {
#ifdef USE_SERIAL
	    Serial.println( fshape.name);
#endif
	    // save this item's offset in the priority array
	    if( fshape.prio < MAX_PRIO && flag == 0) {
	      prio_item[fshape.prio] = fnode.item_offsets[i];

// ESH: delete two lines
//	    if( fshape.prio < 4 && flag == 0) {
//	      ;
	      //	      oled_text_line( fshape.prio, fshape.name);
	      //	      oled_print( fshape.prio, prnt);
	    } else if( flag == 1) {
	      // timezone
	      tzon = fshape.name[0];
	    }
	  }
	}
      } else {		  /* BRANCH */
	search_tree( lat, lon, fnode.node_offsets[i]*sizeof(fnode), ft, fd, fv, fa, flag);
      }
    }
  }
}


// FNV-1a–style hash for uint32_t arrays (by ChatGPT)
uint32_t hash_u32_list(const uint32_t *values, size_t count)
{
    uint32_t hash = 2166136261u;   /* FNV offset basis */

    for (size_t i = 0; i < count; i++) {
        hash ^= values[i];
        hash *= 16777619u;         /* FNV prime */
    }

    return hash;
}

