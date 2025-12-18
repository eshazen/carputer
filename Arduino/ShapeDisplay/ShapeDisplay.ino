//
// Location lookup of R-Tree data and display
//
// currently uses random simulated GPS
//

#include <math.h>
#include <SD.h>
#include <string.h>
#include <avr/dtostrf.h>

#include "oled.h"
#include "shape.h"
#include "filetree.h"

#define RAND_POS

#define USE_OLED

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
char dtos[12];

File ft;
File fd;
File fv;
File fa;

int print_line = 0;

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

  ft = SD.open("STUFF.REE");
  fd = SD.open("STUFF.DAT");
  fv = SD.open("STUFF.VRT");
  fa = SD.open("STUFF.PRT");

  SSD1322_HW_Init();
  SSD1322_API_init();
  select_font( &FreeMono9pt7b);

  pinMode( LED_BUILTIN, OUTPUT);

  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 0, "Display Test 1.0");
  Serial.println("Test 1.0");
  delay(1000);

  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 0, "Display Test 1.1");
  Serial.println("Test 1.0");
  delay(1000);

  fill_buffer( oledBuf, 0);		// clear screen
  oled_print( 0, "Display Test 1.2");
  Serial.println("Test 1.0");
  delay(1000);

} // setup()

void loop() {

  coord_t lat, lon;

  float floatLat, floatLon;

#ifdef RAND_POS
//  const float latMin = 32.0;
//  const float latMax = 42.0;
//
//  const float lonMin = -122.0;
//  const float lonMax = -75.0;

  const float latMin = 42.03;
  const float latMax = 42.67;

  const float lonMin = -71.78;
  const float lonMax = -70.890;

  const float latRange = (latMax-latMin);
  const float lonRange = (lonMax-lonMin);

  long start;

  floatLat = latMin + (float)(random(latRange*10000.0)/10000.0);
  floatLon = lonMin + (float)(random(lonRange*10000.0)/10000.0);
  
  dtostrf( floatLat, 9, 4, prnt);
  Serial.print( prnt);
  Serial.print(" ");
  dtostrf( floatLon, 9, 4, prnt);
  Serial.println( prnt);
#else
  // fixed point near home
  floatLat = 42.3425;
  floatLon = -71.1388;
#endif  
  
  print_line = 1;
  fill_buffer( oledBuf, 0);		// clear screen
  memset( prnt, ' ', sizeof(prnt));
  dtostrf( floatLat, 8, 2, prnt);
  dtostrf( floatLon, 8, 2, dtos);
  strcat( prnt, " ");
  strcat( prnt, dtos);
  prnt[20] = '\0';
  oled_print( 3, prnt); 

  Serial.println("Search tree");
  start = millis();

  search_tree( floatLat, floatLon, 0L, ft, fd, fv, fa);

  long elapsed = millis() - start;
  Serial.print("ms: ");
  Serial.println( elapsed);

  delay(2000);

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
	    Serial.println( prnt);
	    oled_print( 2-fshape.prio, prnt);
	    ++print_line;
	  }
	}
      } else {		  /* BRANCH */
	search_tree( lat, lon, fnode.node_offsets[i]*sizeof(fnode), ft, fd, fv, fa);
      }
    }
  }
}

