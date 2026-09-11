# carputer

## V2.0 Hardware

Looking to design a "low profile" version with bare-metal software and comfortable RAM.

Here is A ChatGPT list... " shortlist of 5–10 currently available MCUs
meeting all of your requirements, including 32/48/64-pin package,
pitch, Flash/RAM, price in small quantities, GCC toolchain, and
exactly how each can be programmed using inexpensive non-proprietary
hardware."

| Rank  | MCU                    | Flash / RAM   | CPU                 | Package         | I/O | Current status | My assessment               |
| ----- | ---------------------- | ------------- | ------------------- | --------------- | --: | -------------- | --------------------------- |
| **1** | **PIC32CK2051GC01064** | 2 MB / 512 KB | Cortex-M33 120 MHz  | 64-TQFP, 0.5 mm |  50 | Active         | ⭐ **Best overall**          |
| **2** | **ATSAME51J20A**       | 1 MB / 256 KB | Cortex-M4F 120 MHz  | 64-TQFP, 0.5 mm |  51 | Active         | ⭐ **Simplest choice**       |
| **3** | **PIC32CZ2051CA70064** | 2 MB / 512 KB | Cortex-M7 300 MHz   | 64-TQFP, 0.5 mm |  44 | Active         | ⭐ **Best performance**      |
| **4** | **PIC32CK2051SG01064** | 2 MB / 512 KB | Cortex-M33F 120 MHz | 64-TQFP, 0.5 mm |  47 | Active         | Best if security matters    |
| **5** | **ATSAMD51J20A**       | 1 MB / 256 KB | Cortex-M4F 120 MHz  | 64-TQFP, 0.5 mm | ~51 | In production  | Excellent, mature ecosystem |
| **6** | **STM32H563RGT6**      | 1 MB / 640 KB | Cortex-M33 250 MHz  | 64-LQFP, 0.5 mm | ~51 | Active         | Excellent ST alternative    |
| **7** | **MSPM33C321A-Q1**     | 1 MB / 256 KB | Cortex-M33 160 MHz  | 64-LQFP, 0.5 mm |  57 | Active/preview | Interesting, but I'd wait   |

I'm leaning towards number 2 or 5.

## V1.0 Hardware

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

**V3.x**

* Hierarchy of search (state first, then state-specific towns?)
* Simple UI (button/knob to enable/disable debug mode)

**Other**

* Categories of POI, selected by knob
  <br>with timeout to return to town display?
* Clock time in local time zone (how to determine time zone?)
* Analog clock display
* Scrolling of long place names?

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

