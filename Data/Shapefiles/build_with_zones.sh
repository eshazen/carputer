#!/bin/bash
./grid_eval cb_2024_us_place_500k -o PLACE
./grid_eval cb_2024_us_county_500k -o COUNTY
./grid_eval cb_2024_us_state_500k -o STATES
./grid_eval Boston_Neighborhood_Boundaries -o BOSTON
./grid_eval Time_Zones -m 'East|Centr|Mount|Paci' -o ZONE
./merge_shape ZONE BOSTON PLACE COUNTY STATES -o STUFFZ
./build_tree STUFFZ
./find_by_tree STUFFZ -L 42.3418 -71.0897
