#ifndef CLI_PROCESS_H_
#define CLI_PROCESS_H_

#include <Arduino.h>
#include "multitasker.h"

#define MAX_PIDS_IN_PROCESS 10

class CliProcess{
public:
    CliProcess();
    ~CliProcess();
    int regTaskPid(pid_t pid);
    int setMultiTasker(MultiTasker *multitasker);
    int destroyRegisteredTasks();
private:
    
public:
    
private:
    pid_t pids[MAX_PIDS_IN_PROCESS];
    MultiTasker *mtasker;
};












#endif //CLI_PROCESS_H_