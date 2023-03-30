#include "multitasker.h"


MultiTasker::MultiTasker(){
    memset(&this->stat, 0, sizeof(this->stat));
    this->free_task = new Task(NULL);
    this->free_task->setPriority(0);
    this->free_task->setStatus(NONE);
    this->free_task->setPid(this->getFreePid());
    for(int i = 0; i < MAX_TASK_NUM; i++){
        this->tasks[i] = this->free_task;
    }
}

int MultiTasker::newTask(void(*task_f)(Task *task), uint8_t priority, task_type_t type = LOOP){
    Task *task = new Task(task_f);
    if (task->changePriority(priority) != 0 || task_f == NULL){
        delete(task);
        return -1;
    }
    task->setType(type);
    task->setStatus(READY);
    task->setPid(this->getFreePid());
    if (this->insertTaskInQueue(task) != 0){
        delete(task);
        return -1;
    }
    this->sortTaskPriorities();
    return task->getPid();
}

pid_t MultiTasker::getFreePid(){
    pid_t pid = PID_START_ON;
    while(1){
        uint8_t used_flag = 0;
		for(int i = 0; i < MAX_TASK_NUM; i++){
            if ((this->tasks[i] != this->free_task) && (this->tasks[i]->getPid() == pid)){
                used_flag = 1;
                break;
            }
        }
        if (!used_flag){
            return pid;
        }
        pid++;
    }
    return -1;
}

void MultiTasker::idle(){
    this->stat.idle_millis += millis() - this->stat.idle_prev_millis;
    this->stat.idle_prev_millis = millis();
    this->sortTaskPriorities();
}

int MultiTasker::insertTaskInQueue(Task *task){
    for (int i=0; i<MAX_TASK_NUM; i++){
        if (this->tasks[i] == this->free_task){
            this->tasks[i] = task;
            return 0;
        }
    }
    return -1;
}

void MultiTasker::sortTaskPriorities(){
    Task *task_buf;
	for(int j = 0; j < MAX_TASK_NUM; j++){
		for(int i = 0; i < MAX_TASK_NUM-1; i++){
			if(this->tasks[i]->getPriority() < this->tasks[i+1]->getPriority()){
				task_buf = this->tasks[i];
				this->tasks[i] = this->tasks[i+1];
				this->tasks[i+1] = task_buf;
			}
		}
	}
}

void MultiTasker::destroyTask(Task *task){
    delete(task);
}

int MultiTasker::destroyTaskAtPid(pid_t pid){
    for (int i=0; i<MAX_TASK_NUM; i++){
        if (this->tasks[i]->getPid() == pid){
            this->tasks[i]->setStatus(DESTROY);
            return 0;
        }
    }
    return -1;
}

int MultiTasker::unblockTaskAtPid(pid_t pid){
    for (int i=0; i<MAX_TASK_NUM; i++){
        if (this->tasks[i]->getPid() == pid){
            this->tasks[i]->setStatus(READY);
            return 0;
        }
    }
    return -1;
}

void MultiTasker::measureCpuLoadTime(){
    uint64_t current_millis = millis();
    if (current_millis - this->stat.load_time_prev_millis >= CPU_LOAD_MEAS_INTERVAL){
        uint16_t load = (uint16_t)(((float)this->stat.idle_millis / CPU_LOAD_MEAS_INTERVAL)*1000);
        if (load > 1000) { load = 1000;}
        this->stat.cpu_load_avg += 1000 - load;
        this->stat.cpu_load_avg_count++;
        if (this->stat.cpu_load_avg_count == CPU_LOAD_AVG_COUNT){
            this->stat.cpu_load = (this->stat.cpu_load_avg / CPU_LOAD_AVG_COUNT) / 10;
            this->stat.cpu_load_avg = 0;
            this->stat.cpu_load_avg_count = 0;
        }
        this->stat.idle_millis = 0;
        this->stat.load_time_prev_millis = current_millis;
    }
}

uint8_t MultiTasker::getCpuLoad(){
    return this->stat.cpu_load;
}

uint8_t MultiTasker::getActiveTaskNum(){
    uint8_t activeTaskNum = 0;
    for (int i = 0; i < MAX_TASK_NUM; i++){
        if (this->tasks[i] != this->free_task){
            activeTaskNum++;
        }
    }
    return activeTaskNum;
}

void MultiTasker::runScheduler(){
    while(1){
        SHED_START:
        uint8_t run_marker = 0;
        this->measureCpuLoadTime();
        for (int i = 0; i < MAX_TASK_NUM; i++){
            if (this->tasks[i] != this->free_task){
                Task *task = this->tasks[i];
                switch (task->getStatus())
                {
                    case READY:
                        task->run();
                        run_marker = 1;
                        if ((i != MAX_TASK_NUM -1) && this->tasks[i+1]->getPriority() < this->tasks[i]->getPriority()){
                            goto SHED_START;
                        }
                        break;
                    case BLOCKED:
                        
                        break;
                    case SLEEP:
                        if (task->checkSleepInterval()){
                            task->setStatus(READY);
                        }
                        break;
                    case YIELD:
                        task->setStatus(READY);
                        break;
                    case DESTROY:
                        destroyTask(this->tasks[i]);
                        this->tasks[i] = this->free_task;
                        break;
                    default:
                        break;
                }
            }
        }
        if (!run_marker){
            this->idle();
        }
    }
}
