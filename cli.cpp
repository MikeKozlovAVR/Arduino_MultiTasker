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
    this->startProcess();

    return 0;
}

int Cli::regProcess(void(*process_f)(Cli *cli), const char* name){
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (!this->process_units[i].used){
            this->process_units[i].process_f = process_f;
            this->process_units[i].name = name;
            this->process_units[i].used = 1;
            return 0;
        }
    }
    return -1;
}

int Cli::startProcess(){
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (this->process_units[i].used){
            if (strcmp(this->process_units[i].name, this->inbuff.buff) == 0){
                this->serial->print("CLI: Process [");
                this->serial->print(this->inbuff.buff);
                this->serial->println("] starting..");
                this->process_units[i].process_f(this);
                return 0;
            }
        }
    }
    this->serial->print("CLI: Process [");
    this->serial->print(this->inbuff.buff);
    this->serial->println("] not found");
    return -1;
}

