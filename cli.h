#ifndef CLI_H_
#define CLI_H_

#include <Arduino.h>
#include <stdint.h>
#include "cli_process.h"

#define CLI_MAX_BUFF_SIZE 32
#define CLI_MAX_PROCESSES 12
#define CLI_MAX_SCRIPT_SIZE 256

typedef struct _inbuff{
    char  buff[CLI_MAX_BUFF_SIZE];
    uint16_t in_count;
}inbuff_t;

typedef enum _process_type{
    PROCT_SINGLE,        //Indicates that this process stoped after execution
    PROCT_CONTINUOUS     //Indicates that this process not stoped after execution
}process_type_t;

typedef struct _process_unit{
    const char* name;
    void(*process_f)(CliProcess *process);
    uint8_t used;
    uint8_t runned;
    process_type_t type;
    CliProcess *cli_proc;
}process_unit_t;

class Cli{
  public:
    Cli();
    void init(HardwareSerial *serial, unsigned long baud);
    int  regProcess(void(*process_f)(CliProcess *process), const char* name, process_type_t type = PROCT_CONTINUOUS);
    void cliSerialEvent();
    size_t out(char* src);
    size_t out(int src);
    size_t out(float src);
    size_t out(float src, uint8_t dec_places);
    int    in(char* str, size_t size);
    int    script(char* src, size_t size);
  private:
    int input(char in);
    int inputProcessing();
    int startProcess(char *proc_name);
    int slayProcess(char *proc_name);
    void startProcess(process_unit_t *proc_unit);
    void slayProcess(process_unit_t *proc_unit);
    void echo();
    void clearInputBuffer();
  public:
    
  private:
    process_unit_t process_units[CLI_MAX_PROCESSES];
    HardwareSerial *serial;
    inbuff_t inbuff;
};



#endif //CLI_H_
