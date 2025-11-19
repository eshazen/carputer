//
// read a shapefile
// build an r-tree
//

// #define VERBOSE

// skip the ray-intersect algorithm
// #define RANGE_ONLY

#include "rtree.h"
#include "shape.h"
#include "shapefil.h"

int point_in_polygon( const coord_t *xvert, const coord_t *yvert, int n, coord_t x, coord_t y);

// approximate limits of lower 48
#define LON_MIN -124.0
#define LON_MAX -66.0
#define LAT_MIN 25.0
#define LAT_MAX 50.0

// starting grid step size
//#define LON_STEP 0.002
//#define LAT_STEP 0.002
#define LON_STEP 0.5
#define LAT_STEP 0.5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char buff[80];

int main(int argc, char **argv) {

  FILE *fo = NULL;
  FILE *fv = NULL;

  int fna = -1;

  if (argc < 2) {
    fprintf( stderr, "Usage: %s input_shapefile_without_extension [-o output]\n", argv[0]);
    return 1;
  }

  for( int i=1; i<argc; i++) {
    printf("Looking at %s\n", argv[i]);
    if( *argv[i] == '-') {
      switch( toupper( argv[i][1])) {
      case 'O':
	if( i == argc-1) {
	  fprintf( stderr, "Missing filename after -O\n");
	  return 1;
	}
	++i;
	snprintf( buff, sizeof( buff), "%s.DAT", argv[i]);
	if( (fo = fopen( buff, "wb")) == NULL) {
	  fprintf( stderr, "Error opening output file %s\n", buff);
	  return 1;
	}
	snprintf( buff, sizeof( buff), "%s.VRT", argv[i]);
	if( (fv = fopen( buff, "wb")) == NULL) {
	  fprintf( stderr, "Error opening output file %s\n", buff);
	  return 1;
	}
	break;
      default:
	fprintf( stderr, "Unknown option: %s\n", argv[i]);
	return 1;
      }
    } else {
      fna = i;
    }
    
  }

  if( fna < 0) {
    fprintf( stderr, "Missing input file name\n");
    return 1;
  }

  SHPHandle hSHP = SHPOpen(argv[fna], "rb");
  if (!hSHP)
    {
      fprintf( stderr, "Error: Could not open SHP file.\n");
      return 1;
    }

  DBFHandle hDBF = DBFOpen(argv[fna], "rb");
  if (!hDBF)
    {
      fprintf( stderr, "Error: Could not open DBF file.\n");
      return 1;
    }

  int nEntities, shapeType;
  double minBound[4], maxBound[4];
  SHPGetInfo(hSHP, &nEntities, &shapeType, minBound, maxBound);

  // allocate an array for the shapes
  a_shape shapes[nEntities];

#ifdef VERBOSE
  printf("===== SHAPEFILE HEADER INFORMATION =====\n");
  printf("Shapefile Base Name: %s\n", argv[fna]);
  printf("Shape Type (numeric): %d\n", shapeType);
  printf("Number of Shapes: %d\n", nEntities);
  printf("Bounding Box:\n");
  printf("  Xmin: %.10g\n  Ymin: %.10g\n  Xmax: %.10g\n  Ymax: %.10g\n",
	 minBound[0], minBound[1], maxBound[0], maxBound[1]);
  printf("  Zmin: %.10g\n  Zmax: %.10g\n", minBound[2], maxBound[2]);
  printf("  Mmin: %.10g\n  Mmax: %.10g\n\n", minBound[3], maxBound[3]);

  int numRecords = DBFGetRecordCount(hDBF);
  int numFields  = DBFGetFieldCount(hDBF);

  printf("===== DBF ATTRIBUTE TABLE HEADER =====\n");
  printf("Number of Attribute Records: %d\n", numRecords);
  printf("Number of Fields: %d\n\n", numFields);

  printf("Fields:\n");
  for (int f = 0; f < numFields; f++)
    {
      char fieldName[12];
      int fieldWidth, fieldDecimals;
      DBFFieldType ftype = DBFGetFieldInfo(hDBF, f, fieldName,
					   &fieldWidth, &fieldDecimals);

      printf("  %-11s  Type=%d  Width=%d  Decimals=%d\n",
	     fieldName, ftype, fieldWidth, fieldDecimals);
    }
  printf("\n");

  printf("===== SHAPE RECORDS =====\n\n");
#endif

  for (int i = 0; i < nEntities; i++) {
    SHPObject *obj = SHPReadObject(hSHP, i);
    if (!obj) continue;

#ifdef VERBOSE      
    printf("Record %d:\n", i);
#endif

    int numFields = DBFGetFieldCount(hDBF);
    for (int f = 0; f < numFields; f++) {
      char fieldName[12];
      int fieldWidth, fieldDecimals;
      DBFGetFieldInfo(hDBF, f, fieldName, &fieldWidth, &fieldDecimals);

      const char *value = DBFReadStringAttribute(hDBF, i, f);
      if (!value) value = "(null)";
#ifdef VERBOSE
      printf("  %s: %s\n", fieldName, value);
#endif
      if( !strcasecmp( fieldName, "NAME"))
	strcpy( buff, value);
      if( !strcasecmp( fieldName, "NAMELSAD"))
	strcpy( buff, value);
    }

    coord_t xMin = 999;
    coord_t xMax = -999;
    coord_t yMin = 999;
    coord_t yMax = -999;
    for (int j = 0; j < obj->nVertices; j++) {
      if( obj->padfX[j] > xMax)
	xMax = obj->padfX[j];
      if( obj->padfX[j] < xMin)
	xMin = obj->padfX[j];
      if( obj->padfY[j] > yMax)
	yMax = obj->padfY[j];
      if( obj->padfY[j] < yMin)
	yMin = obj->padfY[j];
	
#ifdef DEBUG
      printf("  Vertex %d: (%.6f, %.6f)\n",
	     j, obj->padfX[j], obj->padfY[j]);
#endif
    }

#ifdef DEBUG
    printf("nVertices: %d \"%s\" X(%f..%f) Y(%f..%f)\n", obj->nVertices, buff, xMin, xMax, yMin, yMax);
#endif
    shapes[i].minLon = xMin;
    shapes[i].maxLon = xMax;
    shapes[i].minLat = yMin;
    shapes[i].maxLat = yMax;
    strncpy( shapes[i].name, buff, MAX_NAME);

    // allocate and copy the coords
    shapes[i].lat = calloc( obj->nVertices, sizeof(coord_t));
    shapes[i].lon = calloc( obj->nVertices, sizeof(coord_t));
    shapes[i].nvert = obj->nVertices;
    for (int j = 0; j < obj->nVertices; j++) {
      shapes[i].lon[j] = obj->padfX[j];
      shapes[i].lat[j] = obj->padfY[j];
    }

    SHPDestroyObject(obj);
  }
  SHPClose(hSHP);
  if (hDBF) DBFClose(hDBF);

  int inrange = 0;
  int inside = 0;
  int grids = 0;
  int tests = 0;

  int tgrids = ((LAT_MAX-LAT_MIN)/LAT_STEP) * ((LON_MAX-LON_MIN)/LON_STEP);

  printf("Testing %d grids for %d entities...\n", tgrids, nEntities);

  // calculate size of shapes
  int shapsiz = 0;
  shapsiz += sizeof(shapes[0])*nEntities;
  for (int i = 0; i < nEntities; i++)
    shapsiz += 2*sizeof(coord_t)*shapes[i].nvert + strlen(shapes[i].name) + 1;

  printf("Shape data size %d bytes\n", shapsiz);

  // run the grid and see what is inside
  for( coord_t lat = LAT_MIN; lat < LAT_MAX; lat += LAT_STEP) {
    for( coord_t lon = LON_MIN; lon < LON_MAX; lon += LON_STEP) {
      ++grids;
      for (int i = 0; i < nEntities; i++) {
	++tests;
	if( lat >= shapes[i].minLat && lat <= shapes[i].maxLat &&
	    lon >= shapes[i].minLon && lon <= shapes[i].maxLon) {
#ifdef DEBUG
	  printf("Lat %f Lon %f in range for shape %s... ", lat, lon, shapes[i].name);
#endif
	  ++inrange;
#ifndef RANGE_ONLY
	  if( point_in_polygon( shapes[i].lon, shapes[i].lat, shapes[i].nvert, lon, lat)) {
#ifdef DEBUG
	    printf("Inside\n");
#endif
	    ++inside;
	  } else {
#ifdef DEBUG
	    printf("\n");
#endif
	  }
#endif	  
	}
      }
    }
  }

  printf("Tested %d grids against %d shapes\n", grids, nEntities);
  printf("%d total tests, %d in range, %d inside\n", tests, inrange, inside);

  if( fo) {
    //    fwrite( shapes, sizeof( shapes[0]), nEntities, fo);
    write_shapes( shapes, nEntities, fo, fv);
  }

  printf("Creating R-Tree\n");
  struct rtree *tr = rtree_new();

  for (int i = 0; i < nEntities; i++) {
    printf("Insert %d (%f..%f) (%f..%f)\n", i, shapes[i].minLat,shapes[i].maxLat,
	   shapes[i].minLon, shapes[i].maxLon);

    rtree_insert( tr, (double[2]){shapes[i].minLat,shapes[i].minLon},
		  (double[2]){shapes[i].maxLat,shapes[i].maxLon}, &shapes[i]);
  }

  printf("Dumping R-Tree\n");
  rtree_dump( tr);

  return 0;
}
