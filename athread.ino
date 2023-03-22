#include "multitasker.h"
#include "cli.h"

MultiTasker mtasker;
Cli cli_ctrl;
static char report_script[]=
"get_tasks\n"
"get_load\n";

static char starting_script[]=
"get_tasks\n"
"get_load\n"
"start 5\n";

void task_func0(Task *task);
void task_func1(Task *task);
void task_func2(Task *task);
void task_func3(Task *task);
void task_func4(Task *task);

int tsk_count0 = 0;
int tsk_count1 = 0;
int tsk_count2 = 0;
int tsk_count3 = 0;
int tsk_count4 = 0;

void cli_task(Task *task){
  cli_ctrl.cliSerialEvent();
  task->yield();
}

void process_0(CliProcess *process){
  int argc = process->getArgC();
  char **argv = process->getArgV();
  int tasks_num = 0;
  process->setMultiTasker(&mtasker);
  if (argc > 1){
    tasks_num = atoi(argv[1]);
  }
  if (tasks_num > 0){
    process->regTaskPid(mtasker.newTask(task_func0, 15));
  }
  if (tasks_num > 1){
    process->regTaskPid(mtasker.newTask(task_func1, 15));
  }
  if (tasks_num > 2){
    process->regTaskPid(mtasker.newTask(task_func2, 62));
  }
  if (tasks_num > 3){
    process->regTaskPid(mtasker.newTask(task_func3, 62));
  }
  if (tasks_num > 4){
    process->regTaskPid(mtasker.newTask(task_func4, 63));
  }
}

void process_1(CliProcess *process){
  cli_ctrl.out("Process load: ");
  cli_ctrl.out((int)mtasker.getCpuLoad());
  cli_ctrl.out("%\n");
}

void process_2(CliProcess *process){
  cli_ctrl.out("Active Tasks: ");
  cli_ctrl.out((int)mtasker.getActiveTaskNum());
  cli_ctrl.out("\n");
}

void report_process(CliProcess *process){
  cli_ctrl.out("Sys report:\n");
  cli_ctrl.script(report_script, sizeof(report_script));
}

void starting_process(CliProcess *process){
  cli_ctrl.out("Start script:\n");
  cli_ctrl.script(starting_script, sizeof(starting_script));
}

void setup() {
  cli_ctrl.init(&Serial, 9600);
  cli_ctrl.regProcess(process_0, "start");
  cli_ctrl.regProcess(process_1, "get_load", PROCT_SINGLE);
  cli_ctrl.regProcess(process_2, "get_tasks", PROCT_SINGLE);
  cli_ctrl.regProcess(report_process, "report", PROCT_SINGLE);
  cli_ctrl.regProcess(starting_process, "go", PROCT_SINGLE);
  mtasker.newTask(cli_task, 64);
  mtasker.runScheduler();
}

void loop() {
  
}

void task_func0(Task *task){
  cli_ctrl.out((int)task->getPid());
  cli_ctrl.out("\n");
  task->sleep(500);
  /*tsk_count0++;
  if (tsk_count0 == 3){
    tsk_count0 = 0;
    task->destroy();
  }*/
}

void task_func1(Task *task){
  cli_ctrl.out((int)task->getPid());
  cli_ctrl.out("\n");
  task->sleep(1000);
  /*tsk_count1++;
  if (tsk_count1 == 5){
    tsk_count1 = 0;
    task->destroy();
  }*/
}

void task_func2(Task *task){
  cli_ctrl.out((int)task->getPid());
  cli_ctrl.out("\n");
  task->sleep(2000);
  //mtasker.destroyTaskAtPid(104);
  //task->yield();
  /*
  tsk_count2++;
  if (tsk_count2 == 5){
    tsk_count2 = 0;
    task->destroy();
  }*/
}

void task_func3(Task *task){
  cli_ctrl.out((int)task->getPid());
  cli_ctrl.out("\n");
  task->sleep(3000);  
  /*task->yield();
  tsk_count3++;
  if (tsk_count3 == 3){
    tsk_count3 = 0;
    task->destroy();
  }*/
}

void task_func4(Task *task){
  cli_ctrl.out((int)task->getPid());
  cli_ctrl.out("\n");
  task->sleep(2500);
  /*tsk_count4++;
  if (tsk_count4 == 3){
    tsk_count4 = 0;
    task->destroy();
  }*/
}
