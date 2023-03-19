#ifndef CLI_H_
#define CLI_H_

#include <Arduino.h>
#include <stdint.h>
#include "cli_process.h"

#define CLI_MAX_BUFF_SIZE 32
#define CLI_MAX_PROCESSES 12

typedef struct _inbuff{
    char  buff[CLI_MAX_BUFF_SIZE];
    uint16_t in_count;
}inbuff_t;

typedef struct _process_unit{
    const char* name;
    void(*process_f)(CliProcess *cli);
    uint8_t used;
    CliProcess *cli;
}process_unit_t;

class Cli{
  public:
    Cli();
    void init(HardwareSerial *serial, unsigned long baud);
    int  regProcess(void(*process_f)(CliProcess *cli), const char* name);
    void cliSerialEvent();
  private:
    int input(char in);
    int inputProcessing();
    int startProcess(char *proc_name);
    int slayProcess(char *proc_name);
    void echo();
  public:
    
  private:
    process_unit_t process_units[CLI_MAX_PROCESSES];
    HardwareSerial *serial;
    inbuff_t inbuff;
};



#endif //CLI_H_
