//
// rtree representation in a file
//
#include <stdint.h>

#include "shape.h"
#include "rtree.h"

#define LEAF 1
#define BRANCH 2

struct f_node {
  //  enum kind kind;     // LEAF or BRANCH
  uint32_t kind;
  uint32_t count;          // number of rects
  struct rect rects[MAXITEMS];
  union {
    uint32_t node_offsets[MAXITEMS]; /* offset to another node for BRANCH */
    uint32_t item_offsets[MAXITEMS]; /* offset to item for LEAF */
  };
};


