# carputer

Car computer with display and GPS in DIN enclosure

Proposed functions:

* Clock (updated by GPS)
* Nearest town and potentially other geographical features
* Outside temperature

Front panel would have a display, a couple of buttons/knobs
and a USB plug for power from the stereo plus software updates
using Arduino boot loader.

See (Data/README.md)[Data/README.md] for data details and ideas.

## Implementation notes

On the MKR Zero board, there are only a few pins which can be used for
interrupts.  See
[docs](https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/).
These pins are: 0, 1, 4, 5, 6, 7, 8, 9, A1, A2.

Unfortunately the encoder is connected to 11 and 12.  Pin 7 is unused,
but one interrupt doesn't do the trick.

So far no luck getting timer interrupts to work on the MKR zero.
Best bet seems to be the `Adafruit_ZeroTimer` but even though the
example works it seems to crash when trying to run e.g. SPI after setup.

## User Interface

First version displays 3 lines of nearest towns, with a status
line below with GMT time, GPS status and number of satellites.
Controls not implemented yet.

### Enhancements:

* Categories of POI, selected by knob
  <br>with timeout to return to town display?
* Clock time in local time zone (how to determine time zone?)
* Analog clock display
* Scrolling of long place names?

Need to build up a 2nd board for development.

## Resources

See [https://github.com/eshazen/town_ident](https://github.com/eshazen/town_ident)
for a preliminary version based on ATMega328.

## Hardware

* **CPU** - Arduino MKR Zero.  SAMD21 cpu with 256k flash and 32k ram.  32-bit M0 core at 48MHz.
* **Display** - OLED with 3.12 inch 256x64 display.  Amazon:  [https://a.co/d/5zJTbLo](https://a.co/d/5zJTbLo)
* **GPS** - NEO-6M module (Amazon: [link](https://a.co/d/gSQQ6ZD).  The NEO-7M is a higher-performance replacement
* **Antenna** - Bingfu Waterproof "Active" GPS antenna SMA (Amazon: [link](https://a.co/d/aN1YYik).

### Display details

Ordered this 3.12" OLED with 16-pin connector from 
Asked ChatGPT for a pinout and it came up with [this](https://chatgpt.com/share/68c1a13d-e6dc-8000-bd67-d93712c7cb28).

Also see [ER-OLEDM028-1_Interfacing.pdf](Docs/ER-OLEDM028-1_Interfacing.pdf).

Link to
[Datasheet](https://newhavendisplay.com/content/specs/NHD-3.12-25664UCY2.pdf) for DigiKey version.

Beeped out from ours

| Function   | Pin |   | Pin | Function   |
|------------|-----|---|-----|------------|
| GND        | 1   |   | 2   | ?VCC       |
| ?          | 3   |   | 4   | D0 (SCLK?) |
| D1 (SDIN?) | 5   |   | 6   | D2         |
| D3         | 7   |   | 8   | D4         |
| D5         | 9   |   | 10  | D6         |
| D7         | 11  |   | 12  | E/RD       |
| R/W        | 13  |   | 14  | DC         |
| RES        | 15  |   | 16  | CS         |
|            |     |   |     |            |

See https://www.hicenda.com/product/312-inch-oled-module-01.html

Finally, referring to a datasheet: https://docs.rs-online.com/6267/0900766b8162af09.pdf
for the display itself with 30 pin flex, we can wring out the pinout with a meter.

Library candidates:

* [SSD1322_for_Adafruit_GFX](https://github.com/venice1200/SSD1322_for_Adafruit_GFX)
* [SSD1322_OLED_library](https://github.com/wjklimek1/SSD1322_OLED_library)

