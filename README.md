# Arduino_MultiTasker
A simple solution for emulating multitasking in Arduino systems is not a full-fledged operating system (there is no context switching), but it allows you to arrange different tasks as separate functions, set their priority, perform these tasks once or cyclically, thereby emulating the parallel execution.
---
## Classes discription
### class Task
Objects of this class provide the functionality necessary to control the executable tasks.
They store information about the status of the task, its PID, a pointer to a function that is executed as a separate task.
From the point of view of the user interface, it allows you to put a task into sleep mode, give processor time to another task, kill the task, etc.

*Without a service dispatcher class (MultiTasker) that manages the launch of tasks, it is useless.*

### class MultiTasker
A dispatcher class that manages individual tasks, distributes their execution by priority, monitors task wake-up time intervals,
if they are in `sleep()`, measures the percentage of the controller being loaded (by the time spent in the idle state).

---
*The minimum set for the implementation of work in multitasking mode consists of clsss Task and class MultiTasker*

### Examples of using
A simple code example that creates 2 tasks looks like this:
```c++
  #include "multitasker.h"
  
  MultiTasker mtasker;
  
  void task_func1(Task *task){     //Task function 1
    Serial.println(task->getPid());
    //any code, without 'while(1),delay(..) blocking task work.
    task->sleep(500);              //Set task sleep interval after the execution
  }
  void task_func2(Task *task){     //Task function 2
    Serial.println(task->getPid());
    //any code, without 'while(1),delay(..) blocking task work.
    task->sleep(1000);             //Set task sleep interval after the execution
  }
  void setup() {
    Serial.begin(9600);
    mtasker.newTask(task_func1, 10);   //Create the new task into MultiTasker with priority 10 (range 1..64)
    mtasker.newTask(task_func2, 11);   //Create the new task into MultiTasker with priority 11 (range 1..64)
    mtasker.runScheduler();            //Start working task schedule process, its blocking operation.
  }
  
  void loop() {}       //this section is empty
```
When creating a task, you can specify its type,
make it cyclic (that is, so that it continues to run when it is its turn in the scheduler execution queue) or 
specify that this task should be executed once and destroyed:
```c++
  mtasker.newTask(task_func1, 10, SINGLE);   //Create the new task with priority 10, single execution
  ..
  mtasker.newTask(task_func1, 10, LOOP);     //Create the new task with priority 10, cyclic execution
```
As default, task creating with type LOOP.

### Priority order explainig
Task priorities are set in the range from 1 to 64, if a value outside this range is specified, the task will not be created and the function
```c++
  int newTask(void(*task_f)(Task *task), uint8_t priority, task_type_t type = LOOP)
  ```
will return the value -1. If successful, the PID (identifier) of the created task will be returned.

Tasks in the execution queue are sorted in descending order, tasks with a higher priority are executed first, then those with a lower one. Sorting occurs when the dispatcher is just starting to work and at the moment when the dispatcher enters the idle state.

If a higher priority task is running, then all the CPU time will be given only to it until one of the functions is called in this task that allows you to give CPU time to another lower priority task, for example ```task->yield()```, ```task->sleep(. .)```, etc.
In the case when the next task in the queue for execution has the same priority as the current one, then regardless of the called functions, it will be launched (i.e. they will be executed simply cyclically in turn until all tasks of the same priority give CPU time to others) .



---

Kozlov Mikhail (@MikeKozlovAVR)
