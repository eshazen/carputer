# ESRI Shapefiles

* [ESRI Shapefile Technical Description](https://www.esri.com/content/dam/esrisites/sitecore-archive/Files/Pdfs/library/whitepapers/pdfs/shapefile.pdf)
* [Shapelib](http://shapelib.maptools.org/) library
* [Cartographic Boundary Files](https://www.census.gov/geographies/mapping-files/time-series/geo/carto-boundary-file.html)

## Work log

### 2025-11-19

Thinking about a unified data set for point-like and shape-based locations
The R-Tree [wikipedia](https://en.wikipedia.org/wiki/R-tree) seems like
a good data structure to use.

See for example `tidwall/rtree.c` on github.  The challenge here is that
the tree is built dynamically with memory allocated in small chunks
from the heap.  We need to figure out how to store the tree in a file
and access it from there.

Since a custom allocator is provided one could probably make clever
use of this to arrange for data to be stored sequentially when the
tree is built and then written to a file.

The search would have to be modified to reference the file.

### 2025-11-16

Modified `grid_eval.c` to read a shape fileset and write the data in two
output files:

    NAME.DAT - list of fixed-size records, 1 per shape
	NAME.VRT - vertex list for the shapes
	
The `.DAT` file starts with an `int32` record count, followed by
an array of the following:

``` C
typedef struct {
  int nvert;
  char name[MAX_NAME];
  uint32_t lat_off;
  uint32_t lon_off;
  coord_t minLat;
  coord_t minLon;
  coord_t maxLat;
  coord_t maxLon;
} f_shape;
```

(N.B. MAX_NAME currently 80 and `coord_t` is `float`).

The `.VRT` file has two arrays of `float` (for lat and lon)
of `nvert` size for each record.  `lat_off` and `lon_off` provide
the offset into the `.VRT` file.
	
A quick test in `SD_Dump_Places.ino` generates random lat/lon
and searches the entire list from the `places` shape fileset
(32k places).  This takes ~15s per search to compare with min/max
lat/lon.

_Optimisation ideas_:

Sort the places by lat and/or lon so we can do a binary search
(though this is tricky since they vary in size).

Divide country into grid squares.  Create a fixed-size index
file as with Find_Places which points to a list of places for
each grid square.  Places can participate in multiple grids
based on lat/lon min/max.

Try using `int32` scaled lat/lon to see if it is faster.
_Nope, exactly same speed._

_Other thoughts_:

Much of the country is not covered by the `places` dataset.
It would be good to fall back on the old `Find_Places` algorithm
and dataset for anything which doesn't match.  Or, to fall-back
on the counties and/or states datasets.


## Code / algorithm ideas

Goal:  evaluate current lat/lon vs in realtime multiple shapfile datasets and display
the results in some sort of prioritized way.

The shape data for `us_place` is about 33MB (double) or 17MB (float),
including lat/lon, min/max, name, virtex list.

__Plan A__

Pre-compute a lat/lon grid of ~0.1mi resolution for the entire country and
evaluate each grid point.  (This is 360e6 grid points, and would take ~11h
on a single core.).  Not excluded.

Then, write a file of 2^30 int32 with a shape index (2-4GB file!).

* `grid_eval.c` reads a shapefile using `shapelib` and builds a data structure
with the max/min lat+lon, vertex list and name.  Then it traverses the USA in
a lat/lon grid and evaluates each grid point vs the shapes.

This plan seems a bit unrealistic.

__Plan B__

Assign shapes to a coarser grid like the `FindLocs` code does to minimise
the number of shapes to evaluate.  For this, a variant of `grid_eval`
which finds all shapes where min/max fall within a grid square and builds
a data structure of such would work.

## Shapefile data contents/records with possibly useful data

| Name                                    | Use? | Recs  |                         |
|-----------------------------------------|------|-------|-------------------------|
| cb_2024_us_cbsa_500k.shp                | ?    | 935   | Metro/Micro areas       |
| cb_2024_us_county_500k.shp              | ?    | 3235  | Counties                |
| cb_2024_us_place_500k.shp               | Y    | 32612 | Towns                   |
| cb_2024_us_state_500k.shp               | ?    | 56    | State                   |

Not useulf files for reference

| Name                                    | Use? | Recs  |                         |
|-----------------------------------------|------|-------|-------------------------|
| cb_2024_02_anrc_500k.shp                | N    |       |                         |
| cb_2024_50_sdadm_500k.shp               | N    |       |                         |
| cb_2024_72_subbarrio_500k.shp           | N    |       |                         |
| cb_2024_78_estate_500k.shp              | N    |       |                         |
| cb_2024_us_aiannh_500k.shp              | N    |       |                         |
| cb_2024_us_aitsn_500k.shp               | N    |       |                         |
| cb_2024_us_bg_500k.shp                  | N    |       |                         |
| cb_2024_us_cd119_500k.shp               | N    |       | Congressional districts |
| cb_2024_us_concity_500k.shp             | N    |       |                         |
| cb_2024_us_county_within_cd119_500k.shp | N    |       |                         |
| cb_2024_us_cousub_500k.shp              | N    |       | County subdivision      |
| cb_2024_us_csa_500k.shp                 | N    |       |                         |
| cb_2024_us_division_500k.shp            | N    |       |                         |
| cb_2024_us_elsd_500k.shp                | N    |       | School district         |
| cb_2024_us_metdiv_500k.shp              | N    |       |                         |
| cb_2024_us_region_500k.shp              | N    |       |                         |
| cb_2024_us_scsd_500k.shp                | N    |       | School district         |
| cb_2024_us_sldl_500k.shp                | N    |       | Assembly district       |
| cb_2024_us_sldu_500k.shp                | N    |       | State senate            |
| cb_2024_us_tbg_500k.shp                 | N    |       |                         |
| cb_2024_us_tract_500k.shp               | N    |       | Census tract            |
| cb_2024_us_ttract_500k.shp              | N    |       | Tribal ''               |
| cb_2024_us_unsd_500k.shp                | N    |       | School districts        |
|                                         |      |       |                         |

