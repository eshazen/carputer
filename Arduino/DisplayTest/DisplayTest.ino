#include <math.h>
#include <SPI.h>
#include <string.h>
#include "SSD1322_HW_Driver.h"
#include "SSD1322_API.h"
#include "SSD1322_GFX.h"
#include "Fonts/FreeMono12pt7b.h"
#include "Fonts/FreeSansOblique9pt7b.h"

uint8_t buf[OLED_HEIGHT*OLED_WIDTH];

#define NGON_MAX 32
// static int X[NGON_MAX], Y[NGON_MAX];
// 
// #define NGON_WIDTH OLED_WIDTH/2
// #define NGON_HEIGHT OLED_HEIGHT
// 
// #define NGON_X0 (OLED_WIDTH-NGON_WIDTH)
// 
// void ngon( int n) {
//   float a = 0.;
//   float da = (2.0*PI)/n;
//   if( n > NGON_MAX)
//     return;
//   for( int i=0; i<n; i++) {
//     X[i] = ((sin(a)+1.0)/2.0) * (float)NGON_WIDTH + NGON_X0;
//     Y[i] = ((cos(a)+1.0)/2.0) * (float)NGON_HEIGHT;
//     a += da;
//   }
//   fill_buffer( buf, 0);
//   for( int i=0; i<n-1; i++)
//     for( int j=i+1; j<n; j++)
//       draw_line( buf, X[i], Y[i], X[j], Y[j], 15);
//   send_buffer_to_OLED( buf, 0, 0);  
// }

void flush() {
  while( Serial1.available())
    Serial1.read();
}

void setup() {
  pinMode( LED_BUILTIN, OUTPUT);
  SSD1322_HW_Init();
  SSD1322_API_init();
  Serial1.begin(9600);
  Serial.begin(9600);
  select_font( &FreeMono12pt7b);
  flush();
}

char prnt[40]; // one line
char gps[80];

void loop() {

  char *str;
  char *p;
  char *gmt;
  char *date;
  char *stat;
  char *lat;
  char *ns;
  char *lon;
  char *ew;

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

      fill_buffer( buf, 0);
      snprintf( prnt, sizeof(prnt), "%s %s", stat, gmt);
      Serial.println( prnt);
      draw_text( buf, prnt, 1, 15, 15);
      snprintf( prnt, sizeof(prnt), "LAT %10s %s", lat, ns);
      Serial.println( prnt);
      draw_text( buf, prnt, 1, 35, 15);
      snprintf( prnt, sizeof(prnt), "LON %10s %s", lon, ew);
      Serial.println( prnt);
      draw_text( buf, prnt, 1, 55, 15);
      send_buffer_to_OLED( buf, 0, 0);
    }
  }
}
