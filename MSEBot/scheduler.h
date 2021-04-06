
// A schedular helper to run tasks every so often

#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef struct {
  void (*task)(void);
  size_t *period;
  size_t last_run;
  bool *enabled;
} Task;

typedef struct  {
  Task* tasks;
  size_t queued;
} Scheduler;

Scheduler create_scheduler(Task* task_queue) {
  Scheduler s = {
    .tasks = task_queue,
    .queued = 0
  };
  return s;
}

// Adds a task to the schedular
void add_task(Scheduler* scheduler, void (*task)(void), size_t *period, bool *enabled) {
  Task made_task = {
    .task = task,
    .period = period,
    .last_run = 0,
    .enabled = enabled
  };
  (scheduler->tasks)[scheduler->queued] = made_task;
  scheduler->queued++;
}

void run_scheduler(Scheduler* scheduler) {
  for(size_t i = 0; i < scheduler->queued; i++) {
    size_t time = millis();
    Task t = scheduler->tasks[i];
    if(time - t.last_run > *t.period) {
      scheduler->tasks[i].last_run = time;
      (t.task)();
    }
  }
}

#endif
