# carputer

Car computer with display and GPS in DIN enclosure

Proposed functions:

* Clock (updated by GPS)
* Nearest town and potentially other geographical features
* Outside temperature

Front panel would have a display, a couple of buttons/knobs
and a USB plug for power from the stereo plus software updates.

## Resources

### Display

Best bet for display seems to be common 3.12" OLED (256x64).
Legit supplier is DigiKey (NHD-3.12-25664UCY2-ND) for $38.
Link to
[Datasheet](https://newhavendisplay.com/content/specs/NHD-3.12-25664UCY2.pdf).

PCB is 3.5 x 1.75 in (approx)

Library candidates:

* [SSD1322_for_Adafruit_GFX](https://github.com/venice1200/SSD1322_for_Adafruit_GFX)
* [SSD1322_OLED_library](https://github.com/wjklimek1/SSD1322_OLED_library)

### Enclosure

Slot available is single DIN (after removing whatever is in there).
See for example: [this](https://www.thingiverse.com/thing:2090567) single-DIN project.




