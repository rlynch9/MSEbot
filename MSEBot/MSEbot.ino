#include <stdbool.h>

#include "mseduino.h"
#include "motors.h"
#include "hall.h"
#include "tasks.h"
#include "scheduler.h"

/*  PINS
    MSE     ESP   Device
    J11     16    IR Reciever
    J12     17    Left Hall 1
    J13     5     Left Hall 2
    J16     13    Right Hall 1
    J18     14    Right Hall 2
    J19     27    Limit Switch
    J24     12    Left Motor 1
    J25     19    Left Motor 2
    J26     18    Right Motor 1
    J28     4     Left Motor 2
    J29     2     Climbing Motor 1
    J30     15    Climbing Motor 2

*/

#define LIMIT_SWITCH J19

#define START_STATE TURN1

typedef enum {
  WAITING,
  TURN1, // First turn Cw
  DRIVE1, // First drive
  TURN2, // Second turn CCw
  DRIVE2, // Second drive
  SEARCHING, // Find the IR beacon
  DRIVE3, // Drive to the IR Beacon
  CLIMBING,
  HANGING

} State;


State state = WAITING;

size_t state_start_time = 0;

bool prev_state = false;

bool state_init = false;

void toggle_button() {
  bool pressed = !digitalRead(PB1);
  if (pressed == true && pressed != prev_state) {
    if(state == WAITING) {
      change_state(START_STATE);
    } else {
      change_state(WAITING);
    }
  }
  prev_state = pressed;
  state_start_time = millis();
}

void change_state(State new_state) {
  state = new_state;
  state_init = true;
  stopping = false;
  stop_time = 0;
  stop_motors();
}

void setup() {
  pinMode(BRDLED, OUTPUT);
  
  setup_motors();
  setup_tasks();
  Serial.begin(115200);
  setup_halls(J13, J16);
  pinMode(PB1, INPUT_PULLUP);

  pinMode(LIMIT_SWITCH, INPUT);
}

bool found_ir = false;

void loop() {
  drive();
  run_tasks();
  read_ir();
  toggle_button();

  switch(state) {
    case WAITING: break;
    case TURN1: 
      if(state_init) {
        spin(5, S_SPIN, CW);
        state_init = false;
      }
      if(stopped()) {
        change_state(DRIVE1);
      }
    case DRIVE1:
      if(state_init) {
        drive_forward(30);
        state_init = false;
      }
      if(stopped()) {
        change_state(TURN2);
      }
    case TURN2:
      if(state_init) {
        spin(9, S_SPIN, CCW);
        state_init = false;
      }
      if(stopped()) {
        change_state(DRIVE2);
      }
    case DRIVE2:
      if(state_init) {
        drive_forward(70);
        state_init = false;
      }
      if(stopped()) {
        change_state(WAITING);
      }
      break;
    case SEARCHING:
      if(state_init) {
        state_init = false;
        reverse(3);
        STOP_TIME = 10;
      }

      if(stopped() && !found_ir) {
        spin(1, S_SEARCH, CW);
      }

      if(ir_state == IR_SIGNAL_U && !stopping) {
        stop();
        found_ir = true;
      }
      if(stopped() && found_ir) {
        change_state(DRIVE3);
        STOP_TIME = STOP_TIME_DEFAULT;
        found_ir = false;
      }
      break;
    
    case DRIVE3:
      if(state_init) {
        drive_forward(100);
        state_init = false;
      }
      if(ir_state == IR_NO_SIGNAL || ir_state == IR_SIGNAL_A) {
        stop();
      }
      if(ir_state == IR_NO_SIGNAL) {
        Serial.println("Drive -> Search");
        change_state(SEARCHING);
        break;
      }
      if(stopped() && ir_state == IR_SIGNAL_U) {
        state_init = true;
        break;
      }
      if(ir_state == IR_SIGNAL_A) {
        change_state(WAITING);
      }
      break;
    case CLIMBING: 
      if(state_init) {
        climb();
        state_init = false;
      }

      if(digitalRead(LIMIT_SWITCH == HIGH)) {
        stop_climb();
        change_state(HANGING);
      }
      break;
    case HANGING:
      if(state_init) {
        state_init = false;
      }
      if(digitalRead(LIMIT_SWITCH) == LOW) {
        change_state(CLIMBING);
      }
      break;
  }
 
}
