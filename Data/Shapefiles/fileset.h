#ifndef FILESET_H_INCLUDED
#define FILESET_H_INCLUDED

#include <stdio.h>

typedef struct {
  FILE *dat;
  FILE *vrt;
  FILE *prt;
  FILE *ree;
  char fname[128];
} a_fileset;

int open_set( char *name, a_fileset *fs, char *mode);
void close_set( a_fileset *fs);
int file_is_empty( FILE *fp);


#endif
