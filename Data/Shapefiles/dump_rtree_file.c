//
// read an R-Tree file (.REE)
// dump for debug
//

#include <stdio.h>

#include "filetree.h"

char buff[80];

int main( int argc, char *argv[]) {

  FILE *ft;
  FILE *fd;
  f_shape fshape;

  if( argc < 2) {
    fprintf( stderr, "usage: %s fileset\n", argv[0]);
    return 1;
  }

  snprintf( buff, sizeof(buff), "%s.REE", argv[1]);
  ft = fopen( buff, "rb");

  snprintf( buff, sizeof(buff), "%s.DAT", argv[1]);
  fd = fopen( buff, "rb");

  if( !ft || !fd) {
    fprintf( stderr, "Couldn't open %s.REE or %s.DAT\n", argv[1], argv[1]);
    return 1;
  }

  struct f_node fnode;
  int nnum = 0;

  while( fread( &fnode, sizeof(fnode), 1, ft) == 1) {
    printf("NODE: %d type=%d count=%d\n", nnum, fnode.kind, fnode.count);
    for( int i=0; i<fnode.count; i++) {
      printf("  %d: %d\n", i, fnode.node_offsets[i]);
      if( fnode.kind == LEAF) {
	fseek( fd, fnode.item_offsets[i], SEEK_SET);
	fread( &fshape, sizeof(fshape), 1, fd);
	print_fshape( &fshape);
      }
    }
    ++nnum;
  }
  
}
