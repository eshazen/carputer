#ifndef OLED_H_INCLUDED
#define OLED_H_INCLUDED

// maximum number of lines
#define USE_LINES 4

// large font lines
#define LARGE_LINES 3

#define OLED_LINE0_WIDTH 22
#define OLED_LINE_WIDTH 41

void oled_init();
void oled_print( int line, const char *str);
void oled_clear();
void oled_text_clear();
void oled_text_line( int n, char *s);
void oled_text_update();
void oled_text_fill_up();
char *oled_get_text_line( int n);

#endif
