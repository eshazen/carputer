#!/bin/bash
./grid_eval cb_2024_us_place_500k -o PLACE
./grid_eval cb_2024_us_county_500k -o COUNTY
./grid_eval cb_2024_us_state_500k -o STATES
./merge_shape STATES COUNTY PLACE -o STUFF
./build_tree STUFF
./find_by_tree STUFF -L 42.3373 -71.1354
