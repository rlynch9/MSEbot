
#ifndef HALL_H
#define HALL_H

volatile size_t left_hall_ticks = 0;
volatile size_t right_hall_ticks = 0;

void reset_hall_ticks() {
  noInterrupts();
  left_hall_ticks = 0;
  right_hall_ticks = 0;
  interrupts();
}

void left_hall() {
  left_hall_ticks++;
}

void right_hall() {
  right_hall_ticks++;
}

void setup_halls(size_t right, size_t left) {
  attachInterrupt(digitalPinToInterrupt(right), right_hall, RISING);
  attachInterrupt(digitalPinToInterrupt(left), left_hall, RISING);
}

#endif
