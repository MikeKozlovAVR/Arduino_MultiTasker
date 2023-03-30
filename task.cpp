#include "task.h"

#include <stdio.h>

Task::Task(void(*task_f)(Task *task)){
    this->setThreadFunc(task_f);
}

int Task::setThreadFunc(void(*task_f)(Task *task)){
    this->task_f = task_f;
    return 0;
}

task_status_t Task::getStatus(){
    return this->status;
}

void Task::setStatus(task_status_t status){
    this->status = status;
}

void Task::setPriority(uint8_t priority){
    this->priority = priority;
}

uint8_t Task::getPriority(){
    return this->priority;
}

int Task::changePriority(uint8_t priority){
    if (priority > MAX_TASK_PRIORITY || priority == 0){
        return -1;
    }
    this->setPriority(priority);
    return 0;
}

void Task::setType(task_type_t type){
    this->type = type;
}

task_type_t Task::getType(){
    return this->type;
}

void Task::setPid(pid_t pid){
    this->pid = pid;
}

pid_t Task::getPid(){
    return this->pid;
}

int Task::run(){
    this->task_f(this);
    if (this->getType() == SINGLE){
        this->setStatus(DESTROY);
    }
    return 0;
}

void Task::sleep(uint16_t period_ms){
    this->setStatus(SLEEP);
    this->sleep_period = period_ms;
    this->sleep_start_ms = millis();
}

void Task::yield(){
    this->setStatus(YIELD);
}

void Task::destroy(){
    this->setStatus(DESTROY);
}

void Task::blocking(){
    this->setStatus(BLOCKED);
}

unsigned long Task::getSleepMillis(){
    return this->sleep_start_ms;
}

uint16_t Task::getSleepPeriod(){
    return this->sleep_period;
}

uint8_t Task::checkSleepInterval(){
    if (millis() - this->getSleepMillis() >= this->getSleepPeriod()){
        return 1;
    }
    return 0;
}
