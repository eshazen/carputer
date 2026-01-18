#ifndef ENCODER_H_INCLUDED
#define ENCODER_H_INCLUDED

#include <RotaryEncoder.h>

#define ENC_A 12
#define ENC_B 11
#define ENC_GRN 1
#define ENC_RED 2
#define ENC_SW 3

RotaryEncoder *encoder = nullptr;

volatile int iCount = 0;

void checkPosition() {
  encoder->tick();
  ++iCount;
}

void encoder_setup() {
  pinMode( ENC_A, INPUT_PULLUP);
  pinMode( ENC_B, INPUT_PULLUP);
  pinMode( ENC_SW, INPUT_PULLUP);
  pinMode( ENC_RED, OUTPUT);
  pinMode( ENC_GRN, OUTPUT);

  encoder = new RotaryEncoder( ENC_A, ENC_B, RotaryEncoder::LatchMode::FOUR3);
  // this doesn't seem to work for some reason
  attachInterrupt( digitalPinToInterrupt(ENC_A), checkPosition, CHANGE);
  attachInterrupt( digitalPinToInterrupt(ENC_B), checkPosition, CHANGE);
}


#endif
