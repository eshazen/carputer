#include <stdlib.h>
#include <string.h>
#include "fileset.h"

static char buff[80];

int open_set( char *name, a_fileset *fs, char *mode) {
  strncpy( fs->fname, name, sizeof( fs->fname));
  printf( "OPEN %s\n", name);

  snprintf( buff, sizeof(buff), "%s.DAT", name);
  if( (fs->dat = fopen( buff, mode)) == NULL) {
    fprintf( stderr, "Can't open %s\n", buff);
    return 1;
  }
  snprintf( buff, sizeof(buff), "%s.VRT", name);
  if( (fs->vrt = fopen( buff, mode)) == NULL) {
    fprintf( stderr, "Can't open %s\n", buff);
    return 1;
  }
  snprintf( buff, sizeof(buff), "%s.PRT", name);
  if( (fs->prt = fopen( buff, mode)) == NULL) {
    fprintf( stderr, "Can't open %s\n", buff);
    return 1;
  }
  return 0;
}

void close_set( a_fileset *fs) {
  printf( "CLOSE %s\n", fs->fname);
  fclose( fs->dat);
  fclose( fs->vrt);
  fclose( fs->prt);
}

int file_is_empty( FILE *fp) {
  long pos = ftell(fp);
  if( fseek( fp, 0, SEEK_END)) {
    fprintf( stderr, "Seek error\n");
    exit(1);
  }
  if( ftell( fp)) {		/* not empty */
    fseek( fp, pos, SEEK_SET);	/* return to original posn */
    return 0;
  } else
    return 1;
}
