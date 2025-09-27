#!/usr/bin/perl
#
# read census database dump from dbview utility
# convert to C data
#
# input example (excerpted lines):
#
#   Name       : Brookline
#   Intptlat   : +42.3238534
#   Intptlon   : -071.1420033
#
# CSV output to stdout as:
#
#  "Brookline", "MA", +42.3238534, -071.1420033
#
# expects "fips.csv" file available to loop over states
# expects all extracted text files in Text/ named as tl_<year>_<fips>_place.txt
#

use strict;

my $year = "2025";
my $prefix = 1;			# code as prefix

my $nam;
my $bytes = 0;
my $recs = 0;

my $csv = 1;  # set to 1 for CSV

my @lats;
my @lons;
my $lat;
my $lon;

open( my $fips, "<", "fips.csv") or die "Can't find fips.csv: %!";

while( my $fip = <$fips>) {
    chomp $fip;
    my @fi = split /,/, $fip;
    my $name = $fi[0];
    my $abbr = $fi[1];
    my $code = sprintf "%02d", $fi[2];
#    print "name= $name  abbr=$abbr  code=$code\n";
    my $txtf = sprintf "tl_2025_%s_place.txt", $code;
    my $tpath = "Text/$txtf";

    die "Can't find $tpath" if( !-e $tpath);

    open( my $tfp, "<", "$tpath") or die "Can't open $tpath";

    while( my $line = <$tfp>) {

	chomp $line;
	my @f = split /:/, $line;
	if( $line =~ /^Name\s/) {
	    # strip leading space
	    $nam = substr $f[1], 1;
	}
	if( $line =~ /^Intptlat/) {
	    $lat = $f[1];
	}
	if( $line =~ /^Intptlon/) {
	    $lon = $f[1];
	    # simple version for regular C
	    if( $csv) {
		printf( "%s,\"%s\",%s,%s,%s\n", $prefix, $nam, $abbr, $lat, $lon);
	    } else {
		printf( "{ \"%s\", %s, %s},\n",	$nam, $lat, $lon);
	    }
	    $bytes += 8;  # add two floats
	    $bytes += length( $line);
	    $bytes++;
	    $recs++;
	}
    }
}

if( !$csv) {
    print "#define NUM_PLACES $recs\n";
    print "// $bytes bytes used in $recs records\n";
}

