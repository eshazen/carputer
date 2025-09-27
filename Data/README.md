# Data

Downloaded and extracted places database from:
<br>(https://www2.census.gov/geo/tiger/TIGER2023/PLACE/)[https://www2.census.gov/geo/tiger/TIGER2023/PLACE/]

Using perl scripts `get_all_states.pl` and `extract_all_states.pl`.

Each zip file is named with a two-digit "FIPS" code.  These are listed in a nice
table along with the two-letter state abbreviation [here](https://www.bls.gov/respondents/mwr/electronic-data-interchange/appendix-d-usps-state-abbreviations-and-fips-codes.htm).

Converted via spreadsheet to `fips.csv`.

Used perl script `place_to_csv.pl` to create `places.csv`, like so:

```
1,"Fernandina Beach",FL, +30.6579158, -081.4500012
1,"Orlando",FL, +28.4728185, -081.3202421
1,"Zolfo Springs",FL, +27.4925788, -081.7867927
1,"Webster",FL, +28.6131310, -082.0512979
1,"Islamorada, Village of Islands",FL, +24.9843519, -080.5432960
1,"Marathon",FL, +24.7323382, -081.0251218
1,"Layton",FL, +24.8251313, -080.8116238
```

NOTE! There are a few commas inside the quoted strings.

Suggest to include a "type" code so we can later include
other points of interest.

```
Codes:
1    - town name with state
```



