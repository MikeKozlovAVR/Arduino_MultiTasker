#include "multitasker.h"
#include "cli.h"

void task_func0(Task *task);
void task_func1(Task *task);
void task_func2(Task *task);
void task_func3(Task *task);
void task_func4(Task *task);

MultiTasker mtasker;
Cli cli_ctrl;

int tsk_count0 = 0;
int tsk_count1 = 0;
int tsk_count2 = 0;
int tsk_count3 = 0;
int tsk_count4 = 0;

void cli_task(Task *task){
  cli_ctrl.cliSerialEvent();
  task->yield();
}

void process_0(Cli *cli){
  mtasker.newTask(task_func1, 15);
  mtasker.newTask(task_func2, 62);
}

void process_1(Cli *cli){
  Serial.print("Process load: ");
  Serial.print(mtasker.getCpuLoad());
  Serial.println("%");
}

void process_2(Cli *cli){
  Serial.print("Active Tasks: ");
  Serial.println(mtasker.getActiveTaskNum());
}

void setup() {
  cli_ctrl.init(&Serial, 9600);
  cli_ctrl.regProcess(process_0, "start");
  cli_ctrl.regProcess(process_1, "get_load");
  cli_ctrl.regProcess(process_2, "get_tasks");
  mtasker.newTask(cli_task, 64);
  mtasker.runScheduler();
}

void loop() {
  
}

void task_func0(Task *task){
  //Serial.println(task->getPid());
  task->sleep(1000);
  /*tsk_count0++;
  if (tsk_count0 == 3){
    tsk_count0 = 0;
    task->destroy();
  }*/
}

void task_func1(Task *task){
  //Serial.println(task->getPid());
  //delay(50);
  task->sleep(100);
  /*tsk_count1++;
  if (tsk_count1 == 5){
    tsk_count1 = 0;
    task->destroy();
  }*/
}

void task_func2(Task *task){
  //delay(50);
  //Serial.println(task->getPid());
  //mtasker.destroyTaskAtPid(104);
  task->sleep(200);
  //task->yield();
  /*
  tsk_count2++;
  if (tsk_count2 == 5){
    tsk_count2 = 0;
    task->destroy();
  }*/
}

void task_func3(Task *task){
  Serial.println(task->getPid());
  //thread3.sleep(50);  
  task->yield();
  tsk_count3++;
  if (tsk_count3 == 3){
    tsk_count3 = 0;
    task->destroy();
  }
}

void task_func4(Task *task){
  Serial.println(task->getPid());
  task->sleep(2500);
  tsk_count4++;
  if (tsk_count4 == 3){
    tsk_count4 = 0;
    task->destroy();
  }
}
