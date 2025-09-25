#include <SPI.h>
#include "SSD1322_HW_Driver.h"
#include "SSD1322_API.h"
#include "SSD1322_GFX.h"

uint8_t buf[OLED_HEIGHT*OLED_WIDTH];

void init_display() {
  SSD1322_HW_Init();
  SSD1322_API_init();
  draw_rect_filled( buf, 10, 10, 20, 20, 15);
  send_buffer_to_OLED( buf, 0, 0);
}

void setup() {
  pinMode( LED_BUILTIN, OUTPUT);

}

void loop() {
  init_display();
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(100);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(100);                      // wait for a second
}
