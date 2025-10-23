// test the controls
// uses rotary encoder, polled by 250Hz timer interrupt
// from the Adafruit ZeroTimer library
//

#define USE_TIMER

#define USE_SD

#ifdef USE_TIMER
#include <Arduino.h>
#include "Adafruit_ZeroTimer.h"
#endif

#include <RotaryEncoder.h>
#ifdef USE_SD
#include <SPI.h>
#include <SD.h>
const int chipSelect = SDCARD_SS_PIN;
#endif

#define ENC_A 12
#define ENC_B 11
#define ENC_GRN 1
#define ENC_RED 2
#define ENC_SW 3

RotaryEncoder *encoder = nullptr;

#ifdef USE_TIMER
Adafruit_ZeroTimer zerotimer = Adafruit_ZeroTimer(3);

void TC3_Handler() {
  Adafruit_ZeroTimer::timerHandler(3);
}

// the timer callback
volatile bool togglepin = false;
void TimerCallback0(void)
{
  encoder->tick();
}

#endif

void checkPosition() {
  encoder->tick();
}

void setup() {
  // put your setup code here, to run once:
  pinMode( ENC_A, INPUT_PULLUP);
  pinMode( ENC_B, INPUT_PULLUP);
  pinMode( ENC_SW, INPUT_PULLUP);
  pinMode( ENC_RED, OUTPUT);
  pinMode( ENC_GRN, OUTPUT);

  encoder = new RotaryEncoder( ENC_A, ENC_B, RotaryEncoder::LatchMode::FOUR3);

  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  delay(1000);
  
#ifdef USE_TIMER
  Serial.println("Setting up timer");
  delay(1000);

  // Set up the flexible divider/compare
  uint16_t divider  = 1;
  uint16_t compare = 0;
  tc_clock_prescaler prescaler = TC_CLOCK_PRESCALER_DIV1;

  // preset for 250Hz
  divider = 4;
  prescaler = TC_CLOCK_PRESCALER_DIV4;
  compare = (48000000/4)/250.0;
  
  zerotimer.enable(false);
  zerotimer.configure(prescaler,       // prescaler
          TC_COUNTER_SIZE_16BIT,       // bit width of timer/counter
          TC_WAVE_GENERATION_MATCH_PWM // frequency or PWM mode
          );

  zerotimer.setCompare(0, compare);
  zerotimer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, TimerCallback0);
  zerotimer.enable(true);
  Serial.println("timer setup complete");
#endif  


#ifdef USE_SD
  if (!SD.begin(chipSelect)) {
    Serial.println("SD init fail");
  } else {
    Serial.println("SD init OK");
  }

  File root = SD.open("/");
  printDirectory(root, 0);
  Serial.println("done!");

#endif    

  //  attachInterrupt( digitalPinToInterrupt(TEST_PIN), checkPosition, CHANGE);
  //  attachInterrupt( digitalPinToInterrupt(ENC_A), checkPosition, FALLING);
  //  attachInterrupt( digitalPinToInterrupt(ENC_B), checkPosition, FALLING);
  Serial.println("Encoder test");
}

// Read the current position of the encoder and print out when changed.
void loop()
{
  static int pos = 0;

  //  encoder->tick(); // just call tick() to check the state.
  int newPos = encoder->getPosition();

  if (pos != newPos) {
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.println((int)(encoder->getDirection()));
    pos = newPos;
  } // if
} // loop ()




#ifdef USE_SD

void printDirectory(File dir, int numTabs) {

  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

#endif
