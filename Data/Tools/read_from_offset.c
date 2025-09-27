
#include <stdio.h>
#include <stdlib.h>

char buff[255];

int main( int argc, char *argv[]) {

  if( argc < 3) {
    printf("usage: ./read_from_offset <file> <offset>\n");
    exit(1);
  }

  FILE* fp = fopen( argv[1], "rb");
  long offset = atol( argv[2]);

  if( fp == NULL) {
    printf("Can't open %s\n", argv[1]);
    exit(1);
  }

  fseek( fp, offset, SEEK_SET);
  fgets( buff, sizeof(buff), fp);

  printf("%s\n", buff);
}
