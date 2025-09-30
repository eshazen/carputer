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

## Resources

See [https://github.com/eshazen/town_ident](https://github.com/eshazen/town_ident)
for a preliminary version based on ATMega328.

### Processor

* Arduino MKR Zero.  


### Display

Ordered this 3.12" OLED with 16-pin connector from Amazon:  [https://a.co/d/5zJTbLo](https://a.co/d/5zJTbLo)
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

### Enclosure

Slot available is single DIN (after removing whatever is in there).
See for example: [this](https://www.thingiverse.com/thing:2090567) single-DIN project.




