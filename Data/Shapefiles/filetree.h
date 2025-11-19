//
// rtree representation in a file
//

#include "shape.h"
#include "rtree.h"



struct f_node {
  enum kind kind;     // LEAF or BRANCH
  int count;          // number of rects
  struct rect rects[MAXITEMS];
  union {
    long node_offsets[MAXITEMS];
    long item_offsets[MAXITEMS];
  };
};


struct f_rtree {
  struct rect rect;
  long root_offset;
  size_t count;
  size_t height;
};

