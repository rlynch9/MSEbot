
#ifndef IR_DETECTOR_H
#define IR_DETECTOR_H

#include "mseduino.h"

#define IR_RECV J11

void setup_ir() {
  Serial2.begin(2400, SERIAL_8N1, IR_RECV);
}

int8_t read_ir_state() {
  int8_t c = Serial2.read();
  return c;
}

#endif
