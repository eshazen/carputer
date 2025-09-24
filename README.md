# carputer

Car computer with display and GPS in DIN enclosure

Proposed functions:

* Clock (updated by GPS)
* Nearest town and potentially other geographical features
* Outside temperature

Front panel would have a display, a couple of buttons/knobs
and a USB plug for power from the stereo plus software updates
using Arduino boot loader.

## Resources

See [https://github.com/eshazen/town_ident](https://github.com/eshazen/town_ident)
for a preliminary version based on ATMega328.

### Processor

The Arduino MKR Zero seems a good bet, with SAMD21 processor and uSD card on-board.
Ordered one from Amazon.

### Display

Ordered this 3.12" OLED with 16-pin connector from Amazon:  [https://a.co/d/5zJTbLo](https://a.co/d/5zJTbLo)
Asked ChatGPT for a pinout and it came up with [this](https://chatgpt.com/share/68c1a13d-e6dc-8000-bd67-d93712c7cb28).

Also see [ER-OLEDM028-1_Interfacing.pdf](Docs/ER-OLEDM028-1_Interfacing.pdf).

Link to
[Datasheet](https://newhavendisplay.com/content/specs/NHD-3.12-25664UCY2.pdf) for DigiKey version.

Random Arduino forum info:

Pinout for this display reverse-engineered from forums:

| Function  | Pin |   | Pin | Function  |
|-----------|-----|---|-----|-----------|
| VCC       | 1   |   | 2   | GND       |
| GND?      | 3   |   | 4   | nRST      |
| nCS       | 5   |   | 6   | DC        |
| R/W       | 7   |   | 8   | E/RD      |
| SCLK (D0) | 9   |   | 10  | SDIN (D1) |
| D2        | 11  |   | 12  | D3        |
| D4?       | 13  |   | 14  | D5?       |
| D6?       | 15  |   | 16  | D7?       |

ChatGPT Version, totally different

| Function | Pin |   | Pin | Function |
|----------|-----|---|-----|----------|
| GND      | 1   |   | 2   | VCC      |
| NC       | 3   |   | 4   | DC       |
| (D2)     | 5   |   | 6   | (D3)     |
| SCLK     | 7   |   | 8   | SDIN     |
| (D6/D7)  | 9   |   | 10  | (D7)     |
| (test)   | 11  |   | 12  | (VDDIO)  |
| (VDD)    | 13  |   | 14  | E/RD     |
| nCS      | 15  |   | 16  | nRST     |

See https://www.hicenda.com/product/312-inch-oled-module-01.html

This looks similar to ours but the layout is different.
They print the pinout on the silkscreen.

| Function | Pin |   | Pin | Function |
|----------|-----|---|-----|----------|
| GND      | 1   |   | 2   | VDD      |
| NC       | 3   |   | 4   | D0       |
| D1       | 5   |   | 6   | D2       |
| D3       | 7   |   | 8   | D4       |
| D5       | 9   |   | 10  | D6       |
| D7       | 11  |   | 12  | RD       |
| R/W      | 13  |   | 14  | DC       |
| RES      | 15  |   | 16  | CS       |

Finally, referring to a datasheet: https://docs.rs-online.com/6267/0900766b8162af09.pdf
for the display itself with 30 pin flex, we can wring out the pinout with a meter.


Library candidates:

* [SSD1322_for_Adafruit_GFX](https://github.com/venice1200/SSD1322_for_Adafruit_GFX)
* [SSD1322_OLED_library](https://github.com/wjklimek1/SSD1322_OLED_library)

### Enclosure

Slot available is single DIN (after removing whatever is in there).
See for example: [this](https://www.thingiverse.com/thing:2090567) single-DIN project.




