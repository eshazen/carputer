#!/bin/bash
./grid_eval cb_2024_us_place_500k -o PLACE
./grid_eval cb_2024_us_county_500k -o COUNTY
./grid_eval cb_2024_us_state_500k -o STATES
./grid_eval Boston_Neighborhood_Boundaries -o BOSTON
./grid_eval Time_Zones -m 'East|Centr|Mount|Paci' -o ZONE
./merge_shape BOSTON PLACE COUNTY STATES -o STUFF
./build_tree STUFF
./build_tree ZONE
echo "lookup STUFF"
./find_by_tree STUFF -L 42.3373 -71.1354
echo "lookup ZONE"
./find_by_tree ZONE -L 42.3373 -71.1354
