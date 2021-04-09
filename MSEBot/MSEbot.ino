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
    J19     27    PB1
    J20     27    Limit Switch
    J24     12    Left Motor 1
    J25     19    Left Motor 2
    J26     18    Right Motor 1
    J28     4     Left Motor 2
    J21     23    Climbing Motor 1
    J22     25    Climbing Motor 2
*/

#define LIMIT_SWITCH J20

// The states that the bot can be in
// Can be changed through the `change_state` function
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

// The state the robot starts at when the start button is pushed
#define START_STATE TURN1

State state = WAITING;

// Constants to correctly get the toggle button behaviour
size_t state_start_time = 0;
bool prev_state = false;
bool state_init = false;

// Toggle the robot on and off when `PB1` is pressed
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

// Move the robot into a new state
// Has all the setup logic required
void change_state(State new_state) {
  state = new_state;
  state_init = true;
  stopping = false;
  stop_time = 0;
  stop_motors();
}

void setup() {
  Serial.begin(115200);
  pinMode(BRDLED, OUTPUT);
  setup_motors();
  setup_tasks();
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

  // The main robot logic
  switch(state) {
    // Do nothing if the robot is waiting
    case WAITING: break;

    // First turns the robot 90 degrees to the right
    case TURN1: 
      if(state_init) {
        spin(5, S_SPIN, CW);
        state_init = false;
      }
      if(stopped()) {
        change_state(DRIVE1);
      }

    // Drives the robot far enough out to avoid the box
    case DRIVE1:
      if(state_init) {
        drive_forward(30);
        state_init = false;
      }
      if(stopped()) {
        change_state(TURN2);
      }

    // Turn the robot 90 degrees to the left to get around the box
    case TURN2:
      if(state_init) {
        spin(9, S_SPIN, CCW);
        state_init = false;
      }
      if(stopped()) {
        change_state(DRIVE2);
      }

    // Drive the robot to the position to search for the IR Beacon
    case DRIVE2:
      if(state_init) {
        drive_forward(70);
        state_init = false;
      }
      if(stopped()) {
        change_state(SEARCHING);
      }
      break;
    
    // Look for the IR beacon
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
    
    // Drive towards the IR beacon
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
        change_state(CLIMBING);
      }
      break;
    
    // Climb the rope
    case CLIMBING: 
      if(state_init) {
        climb();
        state_init = false;
        Serial.println("Climbing");
      }

      if(digitalRead(LIMIT_SWITCH == HIGH)) {
        stop_climb();
        change_state(HANGING);
        Serial.println("At top");
      }
      break;

    // Stay at the top of the rope
    // If the limit switch is no longer pressed start climbing again
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
