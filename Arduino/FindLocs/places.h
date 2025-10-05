#include <string.h>
#include <strings.h>

//
// definition of "places" data and helper function declarations
//

#define MAX_NAME_LEN 60
#define MAX_STATE_LEN 2

// only static memory here
typedef struct {
  int type;
  char name[MAX_NAME_LEN+1];
  char state[MAX_STATE_LEN+1];
  double lat;
  double lon;
  int lat_grid;
  int lon_grid;
  int used;
} a_place;

int csv_to_place( a_place* ap, char *csv);
int place_to_csv( a_place* p, char *buffer, int buffer_size);
// int free_place( a_place* p);
void dump_place( a_place* p);
void place_to_short( a_place* p, char *buffer, int buffer_size, float distance);
