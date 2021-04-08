
#ifndef TASKS_H
#define TASKS_H

#include "scheduler.h"
#include "mseduino.h"

#define IR_PIN J11

#define IR_NO_SIGNAL 0
#define IR_SIGNAL_U 'U'
#define IR_SIGNAL_A 'A'

Scheduler global;
Task task_list[10];


size_t last_ir_read = 0;
size_t clear_ir_period = 400;
uint8_t ir_state = IR_NO_SIGNAL;

// Invalidates the ir signal if enough time has passed
void clear_ir() {
  if(millis() - last_ir_read > 200) {
    ir_state = IR_NO_SIGNAL;
  }
}

size_t blink_led_period = 500;
bool led_on = false;

// Blinks the robots led as a heartbeat
void blink_led() {
  if(led_on) {
    digitalWrite(BRDLED, LOW);
  } else {
    digitalWrite(BRDLED, HIGH);
  }
  led_on = !led_on;

  if(ir_state == IR_SIGNAL_U) {
    blink_led_period = 200;
  } else if (ir_state == IR_SIGNAL_A) {
    blink_led_period = 100;
  } else {
    blink_led_period = 500;
  }
  
}

void setup_tasks() {
  global = create_scheduler(task_list);
  add_task(&global, &blink_led, &blink_led_period, NULL);
  add_task(&global, &clear_ir, &clear_ir_period, NULL);
  Serial2.begin(2400, SERIAL_8N1, IR_PIN);
}

void run_tasks() {
  run_scheduler(&global);
}

// Reads the ir beacon
void read_ir() {
  int8_t ir = Serial2.read();
  if(ir == 'U') {
    last_ir_read = millis();
    ir_state = IR_SIGNAL_U;
  } else if(ir == 'A') {
    last_ir_read = millis();
    ir_state = IR_SIGNAL_A;
  }
}

#endif
