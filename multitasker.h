#ifndef MULTI_TASKER_H_
#define MULTI_TASKER_H_

#include "task.h"

#define MAX_TASK_NUM 64
#define PID_START_ON 100
#define CPU_LOAD_MEAS_INTERVAL 100
#define CPU_LOAD_AVG_COUNT 20

typedef struct _proc_stat{
    uint8_t cpu_load;
    uint16_t cpu_load_avg;
    uint8_t cpu_load_avg_count;
    uint64_t idle_millis;
    uint64_t idle_prev_millis;
    uint64_t load_time_prev_millis;
}proc_stat_t;

class MultiTasker{
  public:

  private:
    proc_stat_t stat;
    Task *tasks[MAX_TASK_NUM];
    Task *free_task;
  public:
    MultiTasker();
    int  newTask(void(*task_f)(Task *task), uint8_t priority, task_type_t type = LOOP);
    void runScheduler();
    int  destroyTaskAtPid(pid_t pid);
    int  unblockTaskAtPid(pid_t pid);
    uint8_t getCpuLoad();
    uint8_t getActiveTaskNum();
  private:
    pid_t getFreePid();
    int   insertTaskInQueue(Task *task);
    void  idle();
    void  sortTaskPriorities();
    void  destroyTask(Task *task);
    void  measureCpuLoadTime();
};

#endif // MULTI_TASKER_H_