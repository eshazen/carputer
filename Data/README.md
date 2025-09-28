# Data

## Downloading, extraction

Downloaded and extracted places database from:
<br>(https://www2.census.gov/geo/tiger/TIGER2023/PLACE/)[https://www2.census.gov/geo/tiger/TIGER2023/PLACE/]

Using perl scripts `get_all_states.pl` and `extract_all_states.pl`.

Each zip file is named with a two-digit "FIPS" code.  These are listed in a nice
table along with the two-letter state abbreviation [here](https://www.bls.gov/respondents/mwr/electronic-data-interchange/appendix-d-usps-state-abbreviations-and-fips-codes.htm).

Converted via spreadsheet to `fips.csv`.

Used perl script `place_to_csv.pl` to create `places.csv`, like so:

    1,"Fernandina Beach",FL, +30.6579158, -081.4500012
    1,"Orlando",FL, +28.4728185, -081.3202421
    1,"Zolfo Springs",FL, +27.4925788, -081.7867927
    1,"Webster",FL, +28.6131310, -082.0512979
    1,"Islamorada, Village of Islands",FL, +24.9843519, -080.5432960
    1,"Marathon",FL, +24.7323382, -081.0251218
    1,"Layton",FL, +24.8251313, -080.8116238

NOTE! There are a few commas inside the quoted strings.

Suggest to include a "type" code so we can later include
other points of interest.

```
Codes:
1    - town name with state
```

Used a C program `index_file_offsets` to create a list of `long` offsets in
`places.csv` where each line starts.  This could be stored as a binary file
on the uSD card for quick access to any record by record#.

## Processing

We want to display the nearest few items of any given type.
It takes ~60s to traverse `places.csv` and calculate the great circle
distance to each from the current location.

All places are assigned to cells on a grid as described below.
For example, a 0.5 x 0.5 degree cell (25 x 34 miles).

The `places.csv` file is sorted by grid location.
An index file `gindex.dat` contains the offset
of the start of each group in the places file, as a list
of `NumLat*NumLon` 32-bit binary values for fast access.

## Files

### `places.csv`

This file contains a list of all places to be displayed.
Places are assigned to rectangular rid squares (last two columns).

| Number | Name     | Description                     |
|--------|----------|---------------------------------|
| 1      | type     | Type ID (currently all are '1') |
| 2      | name     | Name of place (quoted)          |
| 3      | state    | State abbreviation              |
| 4      | lat      | Latitude, signed decimal        |
| 5      | lon      | Longitude, signed decimal       |
| 6      | lat_grid | Latitude grid cell number       |
| 7      | lon_gird | Longitude grid cell number      |

Sample data:

    Type,Name,State,Lat,Lon,LatG,LonG
    1,"Rehobeth",AL,   31.122548,  -85.439648,55,157
    1,"Coosada",AL,   32.495394,  -86.326543,60,153
    1,"Elmore",AL,   32.556606,  -86.341025,61,153
    1,"Wetumpka",AL,   32.542018,  -86.205307,61,154
    1,"Adamsville",AL,   33.602315,  -86.971527,65,151

### `grid.csv`

This file contains the grid info.
For example, grid (0,0) would be in the range:
<br>Latitude  between 17.25 and 17.50
<br>Longitude between -124.75 and -124.50

    NumLat, LatMin, LatMax, LatStep, NumLon, LonMin, LonMax, LonStep
    130,    17.25,  49.5,   0.25,    243,   -124.75, -64.25, 0.25

