#ifndef ENCODER_H_INCLUDED
#define ENCODER_H_INCLUDED

#include <RotaryEncoder.h>

#define ENC_A 12
#define ENC_B 11
#define ENC_GRN 1
#define ENC_RED 2
#define ENC_SW 3

RotaryEncoder *encoder = nullptr;

void encoder_setup() {
  pinMode( ENC_A, INPUT_PULLUP);
  pinMode( ENC_B, INPUT_PULLUP);
  pinMode( ENC_SW, INPUT_PULLUP);
  pinMode( ENC_RED, OUTPUT);
  pinMode( ENC_GRN, OUTPUT);

  encoder = new RotaryEncoder( ENC_A, ENC_B, RotaryEncoder::LatchMode::FOUR3);
}


#endif
