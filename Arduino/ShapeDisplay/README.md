# ShapeDisplay.ino

### Rev 1.3

* Handle up to 8 priorities, lowest 3 displayed
* Add Boston neighborhoods to the database

### Rev 1.2

Display:
* 3 lines in large font with location info.
* Time (corrected for time zone) at right end of top line.
* Lat/long, date, time, sats on bottom line in smaller font

### Rev 1.0

Initial version of operating sketch.
Expects database in these files on SD card:

    STUFF.DAT   shapes data per shape.h
    STUFF.VRT   virtex list for outlines
    STUFF.PRT   list of outlines
    STUFF.REE   R-Tree

    ZONE.DAT    time zone database
	ZONE.VRT
	ZONE.PRT
	ZONE.REE

The `STUFF` database should have 3 datasets differentiated by
the `prio` item in the shapes data (Town, County, State).
Each is displayed on it's own line.

Expects a GPS connected to Serial1.
If there are no messages coming in, waits forever.

If there is a time message but no location, displays
a fake location along with the time.

If there is a location, lookup in database and display.
