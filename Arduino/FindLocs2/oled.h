#ifndef OLED_H_INCLUDED
#define OLED_H_INCLUDED

#ifdef USE_OLED
#include "SSD1322_HW_Driver.h"
#include "SSD1322_API.h"
#include "SSD1322_GFX.h"
#include "Fonts/FreeMono9pt7b.h"
#include "Fonts/FreeMono12pt7b.h"
#include "Fonts/FreeMono18pt7b.h"

#define LINE_SPC 14
#define USE_LINES 4
uint8_t oledBuf[OLED_HEIGHT*OLED_WIDTH];
#endif

#endif
