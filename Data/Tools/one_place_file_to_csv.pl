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
# usage:  $0 <place_file> <abbr>
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

my $na = $#ARGV + 1;
die "usage: $0 <place_file> <abbr>\n" if( $na < 2);

my $tpath = $ARGV[0];
my $abbr = $ARGV[1];

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
