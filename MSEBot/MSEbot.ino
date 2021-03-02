#include <stdbool.h>

#include "mseduino.h"
#include "motors.h"
#include "hall.h"
#include "tasks.h"
#include "scheduler.h"

#define START_STATE ROTATING_TO_SEARCH_POINT

typedef enum {
  WAITING,
  ROTATING_TO_SEARCH_POINT,
  DRIVING_TO_SEARCH_POINT,
  SEARCHING,
  DRIVING,
  REVERSING,
  FINISHED
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

size_t print_debug_period = 500;
void print_debug() {
  Serial.print("\n\nBot State: ");
  Serial.println(state);
  Serial.print("Drive State (L, R): ");
  Serial.print(left_motor_state);
  Serial.print(", ");
  Serial.println(right_motor_state);
  Serial.print("Halls (L, R): ");
  Serial.print(left_hall_ticks);
  Serial.print(", ");
  Serial.println(right_hall_ticks);
  Serial.print("Stopped? ");
  Serial.println(stopped());
  Serial.print("IR State: ");
  Serial.write(ir_state);
  Serial.println();
  

}

void setup() {
  pinMode(BRDLED, OUTPUT);
  
  setup_motors();
  setup_tasks();
  Serial.begin(115200);
  setup_halls(J13, J16);
  pinMode(PB1, INPUT_PULLUP);
  add_task(&global, &print_debug, &print_debug_period, NULL);
  
}

bool found_ir = false;

void loop() {
  drive();
  run_tasks();
  read_ir();
  toggle_button();

  switch (state) {
    case WAITING: 
      if(state_init) {
        state_init = false;
        stop();
      }
      break;
    case ROTATING_TO_SEARCH_POINT: 
      if(state_init) {
        state_init = false;
        spin(3, S_FORWARD);
      }

      if(stopped()) {
        change_state(DRIVING_TO_SEARCH_POINT);
      }
      break;
    case DRIVING_TO_SEARCH_POINT:
      if(state_init) {
        state_init = false;
        drive_forward(100);
      }
      if(stopped()) {
        change_state(SEARCHING);
      }
      break;
    case SEARCHING:
      if(state_init) {
        state_init = false;
        reverse(3);
        STOP_TIME = 10;
      }

      if(stopped() && !found_ir) {
        spin(1, S_SEARCH);
      }

      if(ir_state == IR_SIGNAL_U && !stopping) {
        stop();
        found_ir = true;
      }
      if(ir_state == IR_SIGNAL_A) {
        change_state(REVERSING);
      }
      if(stopped() && found_ir) {
        change_state(DRIVING);
        STOP_TIME = STOP_TIME_DEFAULT;
        found_ir = false;
      }
      break;
    
    case DRIVING:
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
        change_state(REVERSING);
      }
      break;

    case REVERSING:
      if(state_init) {
        reverse(50);
        state_init = false;
      }
      if(stopped()) {
        change_state(FINISHED);
      }
      break;

    case FINISHED:
      if(state_init) {
        spin(50, 200);
        state_init = false;
      }
      if(stopped()) {
        change_state(WAITING);
      }
      break;
  }
 
}
