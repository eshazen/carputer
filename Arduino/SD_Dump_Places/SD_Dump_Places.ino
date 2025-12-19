//
// read binary shapefile set and print
//

#include <SPI.h>
#include <SD.h>
#include <avr/dtostrf.h>

#include "shape.h"

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

const int chipSelect = SDCARD_SS_PIN;

char buff[80];
char prnt[80];

int count;
f_shape shape;
coord_t coord;

File fp;
File fv;

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

  fp = SD.open("PLACE.DAT");
  fv = SD.open("PLACE.VIR");

  int nlt150 = 0;
  int start = millis();

  int proc = 0;

  int num = 0;

  // read count
  fp.read( &count, sizeof(count));
  Serial.print("NUM SHAPES: ");
  Serial.println( count);

  Serial.print("shape size: ");
  Serial.println( sizeof(shape));

  for( num=0; num<min(10,count); num++) {

    fp.read( &shape, sizeof(shape));
    long cpos = fp.position();
    
    Serial.print("SHAPE: ");
    Serial.print( num);
    
    snprintf( prnt, sizeof(prnt), "%s (%d) ", shape.name, shape.nvert);
    Serial.println( prnt);
    snprintf( prnt, sizeof(prnt), "Lat (%f..%f) Lon: (%f..%f)\n",
	      shape.minLat, shape.maxLat, shape.minLon, shape.maxLon);
    Serial.println( prnt);

    for( int i=0; i<min(5,shape.nvert); i++) {
      fv.read( &coord, sizeof(coord_t));
      dtostrf( coord, 9, 4, prnt);
      Serial.print( prnt);
    }
    Serial.println(" ");
    snprintf( prnt, sizeof(prnt), "  lon: ");
    for( int i=0; i<min(5,shape.nvert); i++) {
      fv.read( &coord, sizeof(coord_t));
      dtostrf( coord, 9, 4, prnt);
      Serial.print( prnt);
    }
    Serial.println( " ");
  }
}

void loop() {

  float floatLat, floatLon;

  const float latMin = 42.25;
  const float latMax = 42.52;

  const float lonMin = -71.2;
  const float lonMax = -71.0;

  const float latRange = (latMax-latMin);
  const float lonRange = (lonMax-lonMin);

  long start = millis();

  // Lat 25..50
  floatLat = latMin + (float)(random(latRange*10000.0)/10000.0);
  // Lon -124..-66
  floatLon = lonMin + (float)(random(lonRange*10000.0)/10000.0);
  
#ifdef INT_COORD
  int32_t intLat = floatLat * COORD_I_SCALE;
  int32_t intLon = floatLon * COORD_I_SCALE;
#endif  

  dtostrf( floatLat, 9, 4, prnt);
  Serial.print( prnt);
  Serial.print(" ");
  dtostrf( floatLon, 9, 4, prnt);
  Serial.println( prnt);

  fp.seek( 4);

  for( int i=0; i<count; i++) {
    if( (i % 5000) == 0)
      Serial.println( i);
    fp.read( &shape, sizeof(shape));
#ifdef INT_COORD
    if( intLat >= shape.minLat && intLat <= shape.maxLat &&
	intLon >= shape.minLon && intLon <= shape.maxLon) {
#else    
    if( floatLat >= shape.minLat && floatLat <= shape.maxLat &&
	floatLon >= shape.minLon && floatLon <= shape.maxLon) {
#endif
      snprintf( prnt, sizeof(prnt),"Found shape %s\n", shape.name);
      Serial.println( prnt);
    }
  }

  long end = millis();

  Serial.print("Elapsed time: ");
  Serial.println( (float)(end-start)/1000.);

}

