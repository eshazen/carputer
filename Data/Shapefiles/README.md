# ESRI Shapefiles

* [ESRI Shapefile Technical Description](https://www.esri.com/content/dam/esrisites/sitecore-archive/Files/Pdfs/library/whitepapers/pdfs/shapefile.pdf)
* [Shapelib](http://shapelib.maptools.org/) library
* [Cartographic Boundary Files](https://www.census.gov/geographies/mapping-files/time-series/geo/carto-boundary-file.html)

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

