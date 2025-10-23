# FindLocs.ino

Initial version of operating sketch.

Read GPS, find grid.
Find all locations in grid plus neighbors.

Loop over all locations, calculate distance
to GPS location.  Insertion sort to make
a list of the n nearest and display.

## Work in Progress

Shaft encoder working using timer interrupt poll at 250Hz.

Reads config file and builds `groups[]`.  For now, hard-wired
to use first group only.
