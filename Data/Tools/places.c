#include <string.h>
#include "parse_csv.h"
#include "places.h"

static char* toks[10];

//
// parse CSV, fill in struct, calloc strings
//
int csv_to_place( a_place* ap, char *csv) {
  int nf = parse_csv_line( csv, toks, sizeof(toks)/sizeof(toks[0]));

  if( nf < 5) {
    fprintf( stderr, "Error in line: %s\n", csv);
    return 1;
  }
  ap->type = atoi( toks[0]);
  if( (ap->name = strdup( toks[1])) == NULL) return 1;
  if( (ap->state = strdup( toks[2])) == NULL) return 1;
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

