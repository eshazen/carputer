#!/usr/bin/perl
#
# Read Hairy FAA APT.txt and extract info
#

while( $line = <>) {
    chomp $line;

    if( $line =~ /^APT/) {
	$ident = substr( $line, 27, 4);
	$lat = substr( $line, 538, 11) / 3600;
	$ns = substr( $line, 549, 1);
	$lat = -$lat if( $ns eq "S");
	$lon = substr( $line, 565, 11) / 3600;
	$ew = substr( $line, 576, 1);
	$lon = -$lon if( $ew eq "W");
	print "$ident $lat $ns $lon $ew\n";
    }
}
