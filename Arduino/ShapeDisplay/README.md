# ShapeDisplay.ino

Initial version of operating sketch.
Expects database in these files on SD card:

    STUFF.DAT   shapes data per shape.h
    STUFF.VRT   virtex list for outlines
    STUFF.PRT   list of outlines
    STUFF.REE   R-Tree

The database should have 3 datasets differentiated by
the `prio` item in the shapes data (Town, County, State).
Each is displayed on it's own line.

Expects a GPS connected to Serial1.
If there are no messages coming in, waits forever.

If there is a time message but no location, displays
a fake location along with the time.

If there is a location, lookup in database and display.
