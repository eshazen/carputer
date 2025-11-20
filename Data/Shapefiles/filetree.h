//
// rtree representation in a file
//
#include <stdint.h>

#include "shape.h"
#include "rtree.h"



struct f_node {
  enum kind kind;     // LEAF or BRANCH
  int count;          // number of rects
  struct rect rects[MAXITEMS];
  union {
    long node_offsets[MAXITEMS]; /* offset to another node for BRANCH */
    long item_offsets[MAXITEMS]; /* offset to item for LEAF */
  };
};


struct f_rtree {
  struct rect rect;		/* overall tree rectangle */
  long root_offset;		/* offset to root node */
  size_t count;			/* number of LEAF items total */
  size_t height;		/* depth of tree */
};

