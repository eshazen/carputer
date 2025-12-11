//
// read a shapefile
//
// write binary DAT and VRT files with the shape database and virtex list for each
// (original purpose was to traverse a grid and count locations in grid)
//

// #define DEBUG

// #define VERBOSE

// skip the ray-intersect algorithm
// #define RANGE_ONLY

#include "shape.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "shapefil.h"

#define MAXMATCH 10

char buff[80];

int verbose = 0;

//
// is GEOID in lower 48?
// return 1 if true, 0 if false
//
int filter_id( int geoID) {
  if( geoID < 60 && geoID != 2 && geoID != 15)
    return 1;
  else
    return 0;
}


// search in string name, look for occurrences of strings in match[0..nmatch-1]
// return index of first string which matches + 1, or 0 if no match
//
int find_match( char *name, char* matches[], int nmatch) {
  for( int i=0; i<nmatch; i++) {
    if( strcasestr( name, matches[i])) {
      return i+1;
    }
  }
  return 0;
}


int main(int argc, char **argv) {

  FILE *fo = NULL;
  FILE *fv = NULL;
  FILE *fa = NULL;

  int fna = -1;
  int maxs = 99999;

  char *matches[MAXMATCH];
  int nmatch = 0;

  int plot = 0;

  int maxVert = 0;			/* max number of vertexes in a part  */

  int filterLower48 = 0;	/* filter the lower 48 GEOID */

  if (argc < 2) {
    fprintf( stderr, 
	     "Usage: %s input_shapefile_without_extension [-f] [-p] [-o output] [-n max] [-m match]\n", argv[0]);
    return 1;
  }

  for( int i=1; i<argc; i++) {
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
	snprintf( buff, sizeof( buff), "%s.PRT", argv[i]);
	if( (fa = fopen( buff, "wb")) == NULL) {
	  fprintf( stderr, "Error opening output file %s\n", buff);
	  return 1;
	}
	break;
      case 'N':
	if( i == argc-1) {
	  fprintf( stderr, "Missing count after -N\n");
	  return 1;
	}
	++i;
	maxs = atoi( argv[i]);
	break;
      case 'M':
	if( i == argc-1) {
	  fprintf( stderr, "Missing match string after -M\n");
	  return 1;
	}
	++i;

	// split argv[i] on "|" into match strings
	matches[nmatch] = strtok( argv[i], "|");
	++nmatch;
	while( (matches[nmatch] = strtok( NULL, "|")) && nmatch < MAXMATCH)
	  ++nmatch;
	break;
      case 'P':
	plot = 1;
	break;
      case 'F':
	filterLower48 = 1;
	break;
      case 'V':
	++verbose;
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

  int nEntities;		/* number of entities in file */
  int savedEntities;		/* number of entities which match */

  int geoID;
  int shapeType;
  double minBound[4], maxBound[4];
  SHPGetInfo(hSHP, &nEntities, &shapeType, minBound, maxBound);
  savedEntities = 0;

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

  savedEntities = 0;

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
      if( !strcasecmp( fieldName, "GEOID")) {
	geoID = atoi( value);
      }
	
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
	
      if( verbose > 1)
	fprintf( stderr, "  Virtex %d: (%.6f, %.6f)\n",
	       j, obj->padfX[j], obj->padfY[j]);
    }

    if( verbose) 
      fprintf( stderr, "nVertexes: %d \"%s\" X(%f..%f) Y(%f..%f)\n", obj->nVertices, buff, xMin, xMax, yMin, yMax);

    if( ((nmatch == 0) || find_match( buff, matches, nmatch)) 
	&& ((filterLower48 == 0) || filter_id( geoID))) {

      if( !plot) fprintf( stderr, "MATCH %s\n", buff);

      shapes[savedEntities].minLon = xMin;
      shapes[savedEntities].maxLon = xMax;
      shapes[savedEntities].minLat = yMin;
      shapes[savedEntities].maxLat = yMax;
      strncpy( shapes[savedEntities].name, buff, MAX_NAME);

      // allocate and copy the coords
      shapes[savedEntities].points = calloc( obj->nVertices, 2*sizeof(coord_t));
      shapes[savedEntities].nvert = obj->nVertices;
      for (int j = 0; j < obj->nVertices; j++) {
	shapes[savedEntities].points[j].lon = obj->padfX[j];
	shapes[savedEntities].points[j].lat = obj->padfY[j];
      }

      fprintf( stderr, "%d parts\n", obj->nParts);

      // copy the list of parts
      shapes[savedEntities].parts = calloc( obj->nParts, sizeof(uint32_t));
      shapes[savedEntities].nparts = obj->nParts;
      for( int j=0; j<obj->nParts; j++)
	shapes[savedEntities].parts[j] = obj->panPartStart[j];

      int p_start, p_end;
      // output parts (distinct polygons) for plotting
      for( int j=0; j<obj->nParts; j++) {
	p_start = obj->panPartStart[j];
	if( j == obj->nParts-1)
	  p_end = obj->nVertices;
	else
	  p_end = obj->panPartStart[j+1];
	if( plot) printf("%d\n", p_end-p_start);
	if( verbose) fprintf( stderr, "Part %d from %d to %d (%d)\n", j, p_start, p_end, p_end-p_start);
	for( int k=p_start; k<p_end; k++) {
	  if( plot) printf("%f %f\n", obj->padfX[k], obj->padfY[k]);
	  ;
	  if( (p_end-p_start)+1 > maxVert)
	    maxVert = (p_end-p_start)+1;
	}
      }

      ++savedEntities;
    }

    SHPDestroyObject(obj);
  }
  SHPClose(hSHP);
  if (hDBF) DBFClose(hDBF);

  if( !plot) {

    if( savedEntities > maxs) {
      fprintf( stderr, "Limiting output to first %d shapes\n", maxs);
      savedEntities = maxs;
    }

    if( fo) {
#ifdef DEBUG
      printf("write_shapes() %d shapes\n", savedEntities);
#endif      
      write_shapes( shapes, savedEntities, fo, fv, fa);
    }
  }

  fprintf( stderr, "Largest shape has %d vertexes\n", maxVert);

  return 0;
}
