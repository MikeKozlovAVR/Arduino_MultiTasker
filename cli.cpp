#include "cli.h"

namespace cli{
static char cli_noti[] = "CLI: ";
static char cli_welcome[] = "Start CLI system\n\r";
static char cli_version[] = "v.1.0.1\n\r";
static char cli_date[] = "date 24.03.23\n\r";
static char slay_procnm[] = "slay";
static char argv_divdr[1] = " ";
}

Cli::Cli(){

}

void Cli::init(HardwareSerial *serial, unsigned long baud){
    this->serial = serial;
    this->serial->begin(baud);
    this->serial->print(cli::cli_welcome);
    this->serial->print(cli::cli_version);
    this->serial->print(cli::cli_date);
}

static uint8_t script_input = 0; 

int Cli::input(char in){
    if (!script_input){
        this->serial->print(in);
    }
    this->inbuff.buff[this->inbuff.in_count] = in;
    if (in == '\r'){
        if (!script_input){
            this->serial->print('\n');
        }
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
    int argc = 0;
    char *argv[CLI_MAX_PROCESS_ARGV];
    char *tmp;
    tmp = strtok(this->inbuff.buff, cli::argv_divdr);
    while (tmp != NULL) {
        if (argc == CLI_MAX_PROCESS_ARGV){
            this->serial->print(cli::cli_noti);
            this->serial->print("argc err {ISBIG}\n\r");
            script_input = 0;
            return -1;
        }
        argv[argc] = tmp;
        argc++;
        tmp = strtok(NULL, cli::argv_divdr);
    }
    if (argc == 2 && strcmp(argv[0], cli::slay_procnm) == 0){
            if (this->slayProcess(argv[1]) != 0){
                this->serial->print(cli::cli_noti);
                this->serial->print(cli::slay_procnm);
                this->serial->print(": [");
                this->serial->print(argv[1]);
                this->serial->print("] not running\n\r");
            }
    }
    else {
        this->startProcess(argv[0], argc, argv);
    }
    script_input = 0;
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

int Cli::startProcess(char *proc_name, int argc, char **argv){
    if (!script_input) { this->serial->print(cli::cli_noti); }
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (this->process_units[i].used){
            if (strcmp(this->process_units[i].name, proc_name) == 0){
                if (!this->process_units[i].runned){
                    if (!script_input) {
                        this->serial->print(proc_name);
                        this->serial->print(" run\n\r");
                    }
                    this->process_units[i].cli_proc->setArg(argc, argv);
                    this->startProcess(&this->process_units[i]);
                    if (this->process_units[i].type == PROCT_SINGLE){
                        this->slayProcess(&this->process_units[i]);
                    }
                } else{
                    this->serial->print("error:[");
                    this->serial->print(proc_name);
                    this->serial->print("] already launched\n\r");
                }
                return 0;
            }
        }
    }
    this->serial->print("Process [");
    this->serial->print(proc_name);
    this->serial->print("] not found\n\r");
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
                this->serial->print("] destroyed\n\r");
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
        this->serial->print("cmd err {ISBIG}\n\r");
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
        this->serial->print("script err {ISBIG}\n\r");
        return -1;
    }
    while (ptr != size){
        for (int i = ptr; i < size; i++){
            if (src[i] == '\r'){
                script_input = 1;
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

