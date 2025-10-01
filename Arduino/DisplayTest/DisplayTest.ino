#include <math.h>
#include <SPI.h>
#include <string.h>
#include "SSD1322_HW_Driver.h"
#include "SSD1322_API.h"
#include "SSD1322_GFX.h"

#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/vga9x167pt7b.h"

#define LINE_SPC 14

uint8_t buf[OLED_HEIGHT*OLED_WIDTH];

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
  select_font( &FreeSans9pt7b);
  //  select_font( &vga_9x167pt7b);
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

      int pos = LINE_SPC;

      fill_buffer( buf, 0);
      snprintf( prnt, sizeof(prnt), "%s %s", stat, gmt);
      Serial.println( prnt);
      draw_text( buf, prnt, 1, pos, 15);
      pos += LINE_SPC;
      snprintf( prnt, sizeof(prnt), "LAT %10s %s", lat, ns);
      Serial.println( prnt);
      draw_text( buf, prnt, 1, pos, 15);
      pos += LINE_SPC;
      snprintf( prnt, sizeof(prnt), "LON %10s %s", lon, ew);
      Serial.println( prnt);
      draw_text( buf, prnt, 1, pos, 15);
      send_buffer_to_OLED( buf, 0, 0);
    }
  }
}
