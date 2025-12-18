//
// display test
//

#include <math.h>
#include <SD.h>
#include <string.h>

#include "oled.h"

void oled_print( int line, const char *str) {
  if( line < USE_LINES) {
    draw_text( oledBuf, str, 1, (line+1) * LINE_SPC, 15);
    send_buffer_to_OLED( oledBuf, 0, 0);
  }
}

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

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

int fake_time = 0;
char gpsFake[32];

void loop() {

  const char *gpsTime;

  fake_time++;
  snprintf( gpsFake, sizeof(gpsFake), "T%d", fake_time);
  gpsTime = gpsFake;

  Serial.println( gpsTime);
  oled_print( 1, gpsTime);

  delay(1000);


} // loop()





