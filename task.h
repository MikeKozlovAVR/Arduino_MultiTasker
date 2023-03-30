#ifndef ATASK_H_
#define ATASK_H_

#include <stdint.h>
#include <Arduino.h>

#define MAX_TASK_PRIORITY 64

typedef enum _task_status{
    NONE,
    READY,
    RUN,
    BLOCKED,
    SLEEP,
    YIELD,
    DESTROY
}task_status_t;

typedef enum _task_type{
    SINGLE,
    LOOP
}task_type_t;

typedef uint16_t pid_t;

class MultiTasker;

class Task{
    friend class MultiTasker;
  public:
    
  private:
    task_status_t status;
    task_type_t   type;
    uint8_t       priority;
    void(*task_f)(Task *task);
    unsigned long sleep_start_ms;
    uint16_t      sleep_period;
    pid_t         pid;
  public:
    void sleep(uint16_t period_ms);
    void yield();
    void destroy();
    void blocking();
    pid_t getPid();
    int changePriority(uint8_t priority);
  private:
    Task(void(*task_f)(Task *task));
    int setThreadFunc(void(*task_f)(Task *task));
    
    void setStatus(task_status_t status);
    void setPriority(uint8_t priority);
    void setType(task_type_t type);
    void setPid(pid_t pid);
    task_status_t getStatus();
    uint8_t       getPriority();
    task_type_t   getType();
    unsigned long getSleepMillis();
    uint16_t      getSleepPeriod();
    
    int run();
    uint8_t checkSleepInterval();
};

#endif //ATASK_H_
