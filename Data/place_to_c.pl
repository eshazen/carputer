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
# output example:
#
#  { "Brookline", +42.3238534, -071.1420033},
#

use strict;

my $nam;
my $bytes = 0;
my $recs = 0;

my $avr = 0;  # set to 1 for AVR, 0 for normal C
my $csv = 1;  # set to 1 for CSV if avr=0

my @lats;
my @lons;
my $lat;
my $lon;

if( $avr) {
    print "// AVR output\n";
    print "#include <avr/pgmspace.h>\n";
}


while( my $line = <>) {
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
	# complicated version for AVR flash memory
	if( $avr) {
	    printf( "const char nam_%d[] PROGMEM = \"%s\";\n", $recs, $nam);
	    $lats[$recs] = $lat;
	    $lons[$recs] = $lon;
	} else {
	    # simple version for regular C
	    if( $csv) {
		printf( "\"%s\",%s,%s\n",	$nam, $lat, $lon);
	    } else {
		printf( "{ \"%s\", %s, %s},\n",	$nam, $lat, $lon);
	    }
	}
	$bytes += 8;  # add two floats
	$bytes += length( $line);
	$bytes++;
	$recs++;
    }
}

if( $avr) {
    # first the latitude array
    printf "const float lats[%s] PROGMEM =\n{\n", $recs;
    my $i;
    for( $i=0; $i<$recs; $i++) {
	if( $i == $recs-1) {
	    printf "  %s  // %d\n", $lats[$i], $i;
	} else {
	    printf "  %s,  // %d\n", $lats[$i], $i;
	}
    }
    print "};\n";
    # now the longitude array
    printf "const float lons[%s] PROGMEM =\n{\n", $recs;
    for( $i=0; $i<$recs; $i++) {
	if( $i == $recs-1) {
	    printf "  %s  // %d\n", $lons[$i], $i;
	} else {
	    printf "  %s,  // %d\n", $lons[$i], $i;
	}
    }
    print "};\n";
    # finally the names index
    print "const char* const name_table[] PROGMEM =\n{\n";
    for( $i=0; $i<$recs; $i++) {
	if( $i == $recs-1) {
	    printf( "nam_%d   // %d\n", $i);
	} else {
	    printf( "nam_%d,  // %d\n", $i);
	}
    }
    print "};\n";
}

if( !$csv) {
    print "#define NUM_PLACES $recs\n";
    print "// $bytes bytes used in $recs records\n";
}

