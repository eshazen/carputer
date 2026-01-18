

#include <Arduino.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

#include "places.h"
#include "parse_csv.h"

static char* toks[10];
static char buffy[256];

//
// parse CSV, fill in struct
//
int csv_to_place( a_place* ap, char *csv) {
  int nf = parse_csv_line( csv, toks, sizeof(toks)/sizeof(toks[0]));

  if( nf < 5) {
    Serial.print( "Error in line: ");
    Serial.println( csv);
    return 1;
  }
  ap->type = atoi( toks[0]);
  strncpy( ap->name, toks[1], MAX_NAME_LEN);
  strncpy( ap->state, toks[2], 2);
  ap->lat = atof( toks[3]);
  ap->lon = atof( toks[4]);
  if( nf > 6) {
    ap->lat_grid = atoi( toks[5]);
    ap->lon_grid = atoi( toks[6]);
  } else {
    ap->lat_grid = -1;
    ap->lon_grid = -1;
  }
  return 0;
}


//
// write place as CSV
//
int place_to_csv( a_place* p, char *buffer, int buffer_size) {
  snprintf( buffer, buffer_size,
	    "%d,\"%s\",%s,%12.8g,%12.8g,%d,%d\n",
	    p->type, p->name, p->state, p->lat, p->lon, p->lat_grid, p->lon_grid);
  return 0;
}


//
// dump place to stdout in compact format
//
void dump_place( a_place* p) {
  place_to_csv( p, buffy, sizeof(buffy));
  Serial.print( "Place: ");
  Serial.println( buffy);
}


//
// format place to buffer
// 
void place_to_short( a_place* p, char *b, int bs, float dist) {

  snprintf( b, bs, "%15.15s %.2s %4.1f", p->name, p->state, dist);
}
