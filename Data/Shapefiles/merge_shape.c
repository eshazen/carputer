//
// merge multiple shape files
//

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "shape.h"
#include "fileset.h"

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

int verbose = 0;

int cat_set( a_fileset *fout, a_fileset *fin, int prio);
void simple_dump( a_fileset *fout);

int main( int argc, char *argv[]) {

  int nsets = 0;
  char *sets[10];
  char *outs = NULL;

  a_fileset fs_in;
  a_fileset fs_out;

  if( argc < 2) {
    fprintf( stderr, "usage: %s <input> [<input>...] -o <output> [-v]\n", argv[0]);
    return 1;
  }

  for( int i=1; i<argc; i++) {
    if( *argv[i] == '-') {
      switch( toupper( argv[i][1])) {
      case 'O':
	if( i == argc-1) {
	  printf("missing output name after -o\n");
	  exit(1);
	}
	++i;
	outs = argv[i];
	break;
      case 'V':
	verbose++;
	break;
      default:
	fprintf( stderr, "unknown option %s\n", argv[i]);
	break;
      }
    } else {
      if( nsets < sizeof(sets)/sizeof(sets[0]))
	sets[nsets++] = strdup( argv[i]);
    }
  }
  
  if( !outs) {
    fprintf( stderr, "Missing output name\n");
    return 1;
  }

  if( open_set( outs, &fs_out, "w+b")) {
    fprintf( stderr, "Error opening output set %s\n", outs);
    return 1;
  }

  for( int set_no=0; set_no<nsets; set_no++) {
    printf("Processing %s\n", sets[set_no]);
    if( open_set( sets[set_no], &fs_in, "rb")) {
      fprintf( stderr, "Error opening input set %s\n", sets[set_no]);
      return 1;
    }

    int ns = cat_set( &fs_out, &fs_in, set_no);
    printf("Set %d: %d shapes concatenated\n", set_no, ns);

    close_set( &fs_in);
  }

  if( verbose)
    simple_dump( &fs_out);

  close_set( &fs_out);
}


//
// concatenate fin data to fout
// copy shapes, write an total count at start of DAT file
// 
int cat_set( a_fileset *fout, a_fileset *fin, int prio) {

  uint32_t in_count, out_count;
  f_shape fshape;
  uint32_t part;
  a_point pt;

  // if output file is empty, write a zero count to get started
  if( file_is_empty( fout->dat)) {
    fprintf( stderr, "File %s.DAT is empty\n", fout->fname);
    out_count = 0;
    fwrite( &out_count, sizeof(out_count), 1, fout->dat);
  }

  // rewind and read current output count
  rewind( fout->dat);
  fread( &out_count, sizeof(out_count), 1, fout->dat);

  // read input count
  fread( &in_count, sizeof(in_count), 1, fin->dat);

  // go to end of output files
  fseek( fout->dat, 0, SEEK_END);
  fseek( fout->prt, 0, SEEK_END);
  fseek( fout->vrt, 0, SEEK_END);

  if( verbose)
    printf("FILE OFFSETS: DAT %ld PRT %ld VRT %ld\n",
	   ftell( fout->dat), ftell( fout->prt), ftell( fout->vrt));

  // copy input data
  for( int num=0; num<in_count; num++) {

    // copy the shape
    if( !fread( &fshape, sizeof(fshape), 1, fin->dat)) {
      fprintf( stderr, "Read error\n");
      exit(1);
    }
    fshape.prio = prio;
    // copy parts
    fshape.part_off = ftell( fout->prt);
    for( int k=0; k<fshape.nparts; k++) {
      fread( &part, sizeof(part), 1, fin->prt);
      fwrite( &part, sizeof(part), 1, fout->prt);
    }

    // copy points
    fshape.points_off = ftell( fout->vrt);
    for( int k=0; k<fshape.nvert; k++) {
      fread( &pt, sizeof(pt), 1, fin->vrt);
      fwrite( &pt, sizeof(pt), 1, fout->vrt);
    }

    if( !fwrite( &fshape, sizeof(fshape), 1, fout->dat)) {
      fprintf( stderr, "Write error\n");
      exit(1);
    }
  }

  // rewrite output count
  out_count += in_count;

  rewind( fout->dat);
  fwrite( &out_count, sizeof(out_count), 1, fout->dat);

  return in_count;
}


//
// simple test dump
//
void simple_dump( a_fileset *fout) {
  uint32_t count;
  f_shape fshape;

  rewind( fout->dat);
  rewind( fout->prt);
  rewind( fout->vrt);
  
  fread( &count, sizeof(count), 1, fout->dat);
  printf( "DUMP %d\n", count);
  for( int i=0; i<count; i++) {
    printf("ITEM %d\n", i);
    fread( &fshape, sizeof(fshape), 1, fout->dat);
    print_fshape( &fshape);
  }
}
