#!/usr/bin/perl
use strict;

# get places data for all states from US gov

for( my $i=1; $i<=78; $i++) {
    #    my $cmd = sprintf "wget https://www2.census.gov/geo/tiger/TIGER2023/PLACE/tl_2023_%02d_place.zip", $i;
    my $cmd = sprintf "wget https://www2.census.gov/geo/tiger/TIGER2025/PLACE/tl_2025_%02d_place.zip", $i;    
    print $cmd, "\n";
    system( $cmd);
}


