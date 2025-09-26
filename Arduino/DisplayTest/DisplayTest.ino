#include <math.h>
#include <SPI.h>
#include "SSD1322_HW_Driver.h"
#include "SSD1322_API.h"
#include "SSD1322_GFX.h"


uint8_t buf[OLED_HEIGHT*OLED_WIDTH];

#define NGON_MAX 32
static int X[NGON_MAX], Y[NGON_MAX];

void ngon( int n) {
  float a = 0.;
  float da = (2.0*PI)/n;
  if( n > NGON_MAX)
    return;
  for( int i=0; i<n; i++) {
    X[i] = ((sin(a)+1.0)/2.0) * (float)OLED_WIDTH;
    Y[i] = ((cos(a)+1.0)/2.0) * (float)OLED_HEIGHT;
    a += da;
  }
  fill_buffer( buf, 0);
  for( int i=0; i<n-1; i++)
    for( int j=i+1; j<n; j++)
      draw_line( buf, X[i], Y[i], X[j], Y[j], 15);
  send_buffer_to_OLED( buf, 0, 0);  
}

void setup() {
  pinMode( LED_BUILTIN, OUTPUT);
  SSD1322_HW_Init();
  SSD1322_API_init();
}

void loop() {
  SSD1322_HW_Init();
  SSD1322_API_init();
  ngon( 5+random(6));
  delay(1000);
}
