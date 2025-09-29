#include <stdio.h>
#include "file_util.h"

int check_clobber( char *files[], int nf) {
  FILE *ft;
  int rc = 0;

  for( int i=0; i<nf; i++) {
    if( (ft = fopen( files[i], "r")) != NULL) {
      fprintf(stderr,"File %s exists!\n", files[i]);
      fclose( ft);
      ++rc;
    }
  }
  return rc;
}
