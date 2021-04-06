
#ifndef MOTORS_H
#define MOTORS_H

#include "mseduino.h"
#include "hall.h"

#define FOREVER ~(0(size_t))

#define TURN_TICKS 6

#define MOTOR_LEFT_A J28
#define MOTOR_LEFT_B J26
#define MOTOR_RIGHT_A J25
#define MOTOR_RIGHT_B J24

#define CLIMB_MOTOR_A J29
#define CLIMB_MOTOR_B J30

#define LEFT_REVERSE 1
#define LEFT_FORWARD 2
#define RIGHT_FORWARD 3
#define RIGHT_REVERSE 4

#define CLIMB_FORWARD 8
#define CLIMB_REVERSE 9

#define S_FORWARD 225
#define S_SPIN 175
#define S_REVERSE S_FORWARD
#define S_SEARCH 125
#define S_STOP 250
#define S_CLIMB 250

#define STOPPED 0
#define FORWARD 1
#define REVERSE 2

typedef enum {
  CW, // Counterclockwise
  CCW // Clockwise
} Rotation;

#define STOP_TIME_DEFAULT 50

size_t STOP_TIME = STOP_TIME_DEFAULT;

uint8_t left_motor_state = STOPPED;
uint8_t right_motor_state = STOPPED;

uint8_t ticks_target = 0;

uint8_t speed = 0;

void setup_motors() {
  ledcAttachPin(MOTOR_LEFT_A, LEFT_REVERSE);
  ledcAttachPin(MOTOR_LEFT_B, LEFT_FORWARD);
  ledcAttachPin(MOTOR_RIGHT_A, RIGHT_FORWARD);
  ledcAttachPin(MOTOR_RIGHT_B, RIGHT_REVERSE);

  ledcSetup(LEFT_REVERSE, 20000, 8);
  ledcSetup(LEFT_FORWARD, 20000, 8);
  ledcSetup(RIGHT_FORWARD, 20000, 8);
  ledcSetup(RIGHT_REVERSE, 20000, 8);

  ledcAttachPin(CLIMB_MOTOR_A, CLIMB_FORWARD);
  ledcAttachPin(CLIMB_MOTOR_B, CLIMB_REVERSE);

  ledcSetup(CLIMB_FORWARD, 20000, 8);
  ledcSetup(CLIMB_REVERSE, 20000, 8);
}

uint8_t reverse_of(uint8_t dir) {
  switch (dir) {
    case STOPPED: return STOPPED;
    case FORWARD: return REVERSE;
    case REVERSE: return FORWARD;
  }
}

void climb() {
  ledcWrite(CLIMB_FORWARD, S_CLIMB);
}

void stop_climb() {
  ledcWrite(CLIMB_FORWARD, 0);
}

void start_drive(uint8_t left_dir, uint8_t right_dir, size_t ticks, uint8_t speed) {
  reset_hall_ticks();
  ticks_target = ticks;
  left_motor_state = left_dir;
  right_motor_state = right_dir;

  switch (left_dir) {
    case FORWARD: 
      ledcWrite(LEFT_FORWARD, speed);
      break;
    case REVERSE:
      ledcWrite(LEFT_REVERSE, speed);
      break;
    //default: break;
  }
  switch (right_dir) {
    case FORWARD: 
      ledcWrite(RIGHT_FORWARD, speed);
      break;
    case REVERSE:
      ledcWrite(RIGHT_REVERSE, speed);
      break;
    //default: break;
  }
}

void drive_forward(size_t ticks) {
  start_drive(FORWARD, FORWARD, ticks, S_FORWARD);
}

void reverse(size_t ticks) {
  start_drive(REVERSE, REVERSE, ticks, S_REVERSE);
}

void spin(size_t ticks, uint8_t speed, Rotation rot) {
  if(rot == CCW) {
    start_drive(REVERSE, FORWARD, ticks, speed);
  } else {
    start_drive(FORWARD, REVERSE, ticks, speed);
  }
}

bool stopped() {
  return left_motor_state == STOPPED && right_motor_state == STOPPED;
}

size_t stop_time = 0;
bool stopping = false;

void stop_motors() {
  ledcWrite(LEFT_FORWARD, 0);
  ledcWrite(LEFT_REVERSE, 0);
  ledcWrite(RIGHT_FORWARD, 0);
  ledcWrite(RIGHT_REVERSE, 0);

  left_motor_state = STOPPED;
  right_motor_state = STOPPED;
}

void stop() {

  reset_hall_ticks();
  
  if(stopped()) {
    return;
  }
  
  stopping = true;
  stop_time = millis() + STOP_TIME;

  uint8_t left_dir = reverse_of(left_motor_state);
  uint8_t right_dir = reverse_of(right_motor_state);

  left_motor_state = left_dir;
  right_motor_state = right_dir;

  switch (left_dir) {
    case FORWARD: 
      ledcWrite(LEFT_FORWARD, S_STOP);
      break;
    case REVERSE:
      ledcWrite(LEFT_REVERSE, S_STOP);
      break;
  }
  switch (right_dir) {
    case FORWARD: 
      ledcWrite(RIGHT_FORWARD, S_STOP);
      break;
    case REVERSE:
      ledcWrite(RIGHT_REVERSE, S_STOP);
      break;
  }
}

void drive() {
  
  if(left_motor_state == STOPPED && right_motor_state == STOPPED) {
    return;
  }
  if(left_hall_ticks > ticks_target || right_hall_ticks > ticks_target) {
    stop();
  }

  if(stopping && millis() > stop_time) {
    stopping = false;
    stop_motors();
  }
}


#endif
