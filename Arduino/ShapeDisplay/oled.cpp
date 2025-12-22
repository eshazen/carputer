
#include "oled.h"

#include <string.h>

#define PROGMEM

#include "SSD1322_HW_Driver.h"
#include "SSD1322_API.h"
#include "SSD1322_GFX.h"
#include "Fonts/FreeMono9pt7b.h"
#include "Fonts/Font5x7FixedMono.h"

// parameters for two fonts, top one large, 5 lines below smaller
const GFXfont *f1 = &FreeMono9pt7b;     // top line font
const GFXfont *f2 = &Font5x7FixedMono;	// subsequent lines font
const int dpy_top = 12;	// initial cursor posn
const int dpy_adv1 = 16;	// top line advance
const int dpy_adv2 = 12;	// subsequent lines advance

static int cpos;
static uint8_t oledBuf[OLED_HEIGHT*OLED_WIDTH];

// display buffers for up to 5 lines
static char dpy_line[USE_LINES][41];

// return pointer to text line
char *oled_get_text_line( int n) {
  return &dpy_line[n][0];
}

// shift up if top line is empty
void oled_text_fill_up() {
  if( !dpy_line[0][0]) {	// this line is empty
    for( int k=1; k<LARGE_LINES; k++) {
      strncpy( dpy_line[k-1], dpy_line[k], OLED_LINE_WIDTH);
    }
    dpy_line[LARGE_LINES-1][0] = '\0';
  }
}

// render the text buffers on the display
void oled_text_update() {
  oled_clear();
  for( int i=0; i<USE_LINES; i++) {
    if( !dpy_line[i][0])
      strcpy( dpy_line[i], "-");
    oled_print( i, dpy_line[i]);
  }
}

// clear the text buffer
void oled_text_clear() {
  for( int i=0; i<USE_LINES; i++)
    dpy_line[i][0] = '\0';
}

// copy text to line
void oled_text_line( int n, char *s) {
  strncpy( &dpy_line[n][0], s, OLED_LINE_WIDTH);
}

// clear the actual display buffer
void oled_clear() {
  fill_buffer( oledBuf, 0);
}

// print to line <line> 0..USE_LINES-1
//   0..LARGE_LINES-1  : use large font
//   remainder : use small font
void oled_print( int line, const char *str) {
  if( line >= USE_LINES)
    return;
  if( line < LARGE_LINES) {	// large line
    select_font( &FreeMono9pt7b);
    cpos = dpy_adv1 * (line + 1);
    draw_text( oledBuf, str, 0, cpos, 15);
  } else {			// small line
    select_font( &Font5x7FixedMono);    
    cpos = dpy_adv1 * (LARGE_LINES+1) + dpy_adv2 * (line - LARGE_LINES);
    draw_text( oledBuf, str, 0, cpos, 15);
  }

  send_buffer_to_OLED( oledBuf, 0, 0);
}

void oled_init() {
  SSD1322_HW_Init();
  SSD1322_API_init();
  select_font( &FreeMono9pt7b);
}
