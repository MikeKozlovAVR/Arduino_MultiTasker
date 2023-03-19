#include "cli.h"



Cli::Cli(){

}

void Cli::init(HardwareSerial *serial, unsigned long baud){
    this->serial = serial;
    this->serial->begin(baud);
}

int Cli::input(char in){
    this->inbuff.buff[this->inbuff.in_count] = in;
    if (in == '\n'){
        this->inbuff.buff[this->inbuff.in_count] = '\0';
        this->inputProcessing();
        memset(this->inbuff.buff, 0, sizeof(this->inbuff.buff));
        this->inbuff.in_count = 0;
        return 0;
    }
    this->inbuff.in_count++;
    if (this->inbuff.in_count >= CLI_MAX_BUFF_SIZE){
        memset(this->inbuff.buff, 0, sizeof(this->inbuff.buff));
        this->inbuff.in_count = 0;
    }
    return 0;
}

void Cli::cliSerialEvent(){
    while(this->serial->available() > 0){
        this->input(this->serial->read());
    }
}

void Cli::echo(){
    this->serial->println(this->inbuff.buff);
}

int Cli::inputProcessing(){
    //this->echo();
    char cmd_buf[CLI_MAX_BUFF_SIZE] = {0};
    char arg_buf[CLI_MAX_BUFF_SIZE] = {0};
    int command_div = 0;
    for (int i=0; i<this->inbuff.in_count; i++){
        if (this->inbuff.buff[i] == ' '){
            command_div = i;
            break;
        }
    }
    if (command_div != 0){
        memcpy(cmd_buf, this->inbuff.buff, command_div);
        memcpy(arg_buf, this->inbuff.buff + command_div + 1, this->inbuff.in_count - command_div - 1);
        if (strcmp(cmd_buf, "slay") == 0){
            if (this->slayProcess(arg_buf) != 0){
                this->serial->print("CLI slay: Process [");
                this->serial->print(arg_buf);
                this->serial->println("] not found");
            }
        }
        else {
            this->startProcess(cmd_buf);
        }
    }
    else {
        this->startProcess(this->inbuff.buff);
    }

    return 0;
}

int Cli::regProcess(void(*process_f)(CliProcess *cli), const char* name){
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (!this->process_units[i].used){
            this->process_units[i].process_f = process_f;
            this->process_units[i].name = name;
            this->process_units[i].used = 1;
            this->process_units[i].cli = new CliProcess();
            return 0;
        }
    }
    return -1;
}

int Cli::startProcess(char *proc_name){
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (this->process_units[i].used){
            if (strcmp(this->process_units[i].name, proc_name) == 0){
                this->serial->print("CLI: Process [");
                this->serial->print(proc_name);
                this->serial->println("] starting..");
                this->process_units[i].process_f(this->process_units[i].cli);
                return 0;
            }
        }
    }
    this->serial->print("CLI: Process [");
    this->serial->print(proc_name);
    this->serial->println("] not found");
    return -1;
}

int Cli::slayProcess(char *proc_name){
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (this->process_units[i].used){
            if (strcmp(this->process_units[i].name, proc_name) == 0){
                this->process_units[i].cli->destroyRegisteredTasks();
                //delete(this->process_units[i].cli);
                //this->process_units[i].process_f = NULL;
                //this->process_units[i].used = 0;
                this->serial->print("CLI: Process [");
                this->serial->print(proc_name);
                this->serial->println("] destroyed");
                return 0;
            }
        }
    }
    return -1;
}

