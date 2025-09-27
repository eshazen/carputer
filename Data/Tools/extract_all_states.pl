#!/usr/bin/perl

# unzip each state, extract DB, convert to text and clean up
# loop over known states in fips.csv
#
# Expects zipfiles in States/
# Outputs textfiles in Text/
# 
use strict;

open( my $fips, "<", "fips.csv") or die "Can't find fips.csv: %!";

while( my $fip = <$fips>) {
    chomp $fip;
    my @fi = split /,/, $fip;
    my $name = $fi[0];
    my $abbr = $fi[1];
    my $code = sprintf "%02d", $fi[2];
    print "name= $name  abbr=$abbr  code=$code\n";
    my $zipf = sprintf "tl_2025_%s_place.zip", $code;
    if( -e "States/$zipf") {
 	my $dbf = sprintf "tl_2025_%s_place.dbf", $code;
 	my $txt = sprintf "tl_2025_%s_place.txt", $code;
	my $glob = sprintf "tl_2025_%s_place.*", $code;

 	my $cmd = "unzip States/$zipf";
 	print "$cmd\n";
 	system( $cmd);
 
 	$cmd = "dbview $dbf > Text/$txt";
 	print "$cmd\n";
 	system( $cmd);
 
 	$cmd = "rm $glob";
 	print "$cmd\n";
 	system( $cmd);
     } else {
 	print("$zipf missing\n");
    }
}
