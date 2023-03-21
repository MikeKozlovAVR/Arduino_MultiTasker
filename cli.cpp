#include "cli.h"

namespace cli{
static char cli_noti[] = "CLI: ";
}

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
        this->clearInputBuffer();
        return 0;
    }
    this->inbuff.in_count++;
    if (this->inbuff.in_count >= CLI_MAX_BUFF_SIZE){
        this->clearInputBuffer();
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
                this->serial->print(cli::cli_noti);
                this->serial->print("slay: [");
                this->serial->print(arg_buf);
                this->serial->println("] not running");
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

int Cli::regProcess(void(*process_f)(CliProcess *process), const char* name, process_type_t type = PROCT_CONTINUOUS){
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (!this->process_units[i].used){
            this->process_units[i].process_f = process_f;
            this->process_units[i].name = name;
            this->process_units[i].type = type;
            this->process_units[i].used = 1;
            this->process_units[i].cli_proc = new CliProcess();
            return 0;
        }
    }
    return -1;
}

int Cli::startProcess(char *proc_name){
    this->serial->print(cli::cli_noti);
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (this->process_units[i].used){
            if (strcmp(this->process_units[i].name, proc_name) == 0){
                if (!this->process_units[i].runned){
                    this->serial->print(proc_name);
                    this->serial->println(" run");
                    this->startProcess(&this->process_units[i]);
                    if (this->process_units[i].type == PROCT_SINGLE){
                        this->slayProcess(&this->process_units[i]);
                    }
                } else{
                    this->serial->print("error:[");
                    this->serial->print(proc_name);
                    this->serial->println("] already launched");
                }
                return 0;
            }
        }
    }
    this->serial->print("Process [");
    this->serial->print(proc_name);
    this->serial->println("] not found");
    return -1;
}

void Cli::startProcess(process_unit_t *proc_unit){
    proc_unit->runned = 1;
    proc_unit->process_f(proc_unit->cli_proc);
}

int Cli::slayProcess(char *proc_name){
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (this->process_units[i].used && this->process_units[i].runned){
            if (strcmp(this->process_units[i].name, proc_name) == 0){
                this->slayProcess(&this->process_units[i]);
                this->serial->print(cli::cli_noti);
                this->serial->print("Process [");
                this->serial->print(proc_name);
                this->serial->println("] destroyed");
                return 0;
            }
        }
    }
    return -1;
}

void Cli::slayProcess(process_unit_t *proc_unit){
    proc_unit->runned = 0;
    proc_unit->cli_proc->destroyRegisteredTasks();
}

void Cli::clearInputBuffer(){
    memset(this->inbuff.buff, 0, sizeof(this->inbuff.buff));
    this->inbuff.in_count = 0;
}

size_t Cli::out(char* src){
    return this->serial->print(src);
}

size_t Cli::out(int src){
    return this->serial->print(src);
}

size_t Cli::out(float src){
    return this->serial->print(src);
}

size_t Cli::out(float src, uint8_t dec_places){
    return this->serial->print(src, dec_places);
}

int Cli::in(char* str, size_t size){
    if (size > CLI_MAX_BUFF_SIZE){
        this->serial->print(cli::cli_noti);
        this->serial->println("cmd err {ISBIG}");
        return -1;
    }
    for (int i = 0; i < size; i++){
        this->input(str[i]);
    }
    return 0;
}

int Cli::script(char* src, size_t size){
    int ptr = 0;
    char* tmp = src;
    this->clearInputBuffer();
    if (size > CLI_MAX_SCRIPT_SIZE){
        this->serial->print(cli::cli_noti);
        this->serial->println("script err {ISBIG}");
        return -1;
    }
    while (ptr != size){
        for (int i = ptr; i < size; i++){
            if (src[i] == '\n'){
                if(this->in(tmp, i - ptr + 1) != 0){
                    return -1;
                }
                ptr = i + 1;
                tmp = src + ptr;
                break;
            }
            else if (src[i] == '\0'){
                return 0;
            }
        }
    }
    return 0;
}

