#!/usr/bin/perl
#

my $geoid;
my $name;

while( $line = <>) {
    chomp $line;
    
    if( $line =~ /GEOID:/) {
	($geoid) = $line =~ /GEOID: (.*)$/;
#	print "GEOID line: $line\n";
#	print "parsed: $geoid\n";
    }
    if( $line =~ /NAME:/) {
#	print "NAME line: $line\n";
	($name) = $line =~ /NAME: (.*)$/;
#	print "parsed: $name\n";
    }
    if( $line =~ /parts/) {
	printf "$geoid $name\n";
    }
}
