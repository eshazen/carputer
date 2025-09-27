//
// read places file and sort
//

#include <SPI.h>
#include <SD.h>
#include "distance_miles.h"

const int chipSelect = SDCARD_SS_PIN;

char buff[80];
char prnt[80];

float here_lat = 42.34228;
float here_lon = -71.13778;

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

  File fp = SD.open("places.csv");

  int nlt150 = 0;
  int start = millis();

  int proc = 0;

  // read some lines
  while ( fp.available()) {
    int n = fp.readBytesUntil( 0xa, buff, sizeof(buff));
    ++proc;
    if( (proc % 100) == 0)
      Serial.println(proc);
    buff[n] = '\0';
    //    Serial.println( buff);
    char* name = strtok( buff, ",");
    float lat = atof(strtok( NULL, ","));
    float lon = atof(strtok( NULL, ","));

    float miles = distance_miles( here_lat, here_lon, lat, lon);
    if( miles < 150)
      ++nlt150;

    //    snprintf( prnt, sizeof(prnt), "NAME=%s lat=%f lon=%f dist=%f\n", name, lat, lon, miles);
    //    Serial.print( prnt);
    
  }

  int end = millis();
  snprintf( prnt, sizeof(prnt), "%d names in %d ms", nlt150, end-start);
  Serial.println( prnt);

}

void loop() {
}

