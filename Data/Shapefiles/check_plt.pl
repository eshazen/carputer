#!/usr/bin/perl
#
# check a plot file

$count = 0;

while( $line = <>) {
    chomp $line;
    if( $line =~ /^\d+$/) {	# count
	if( $count) {
	    print "Error: leftover count $count\n";
	    exit;
	}
	($count) = $line =~ /^(\d+)$/;
	print "count = $count\n";
	if( $count == 0) {
	    print "ERROR: zero count\n";
	    exit;
	}
    } elsif( $line =~ /^[0-9.-]+ [0-9.-]+$/) { # coords
	($x,$y) = $line =~ /^([0-9.-]+) ([0-9.-]+)$/; # coords
	if( $count == 0) {
	    print "Error:  coords seen without active count\n";
	    exit;
	}
	print "$count coords: $x $y\n";
	--$count;
    }
}
