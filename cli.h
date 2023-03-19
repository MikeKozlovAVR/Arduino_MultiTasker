#ifndef CLI_H_
#define CLI_H_

#include <Arduino.h>
#include <stdint.h>

#define CLI_MAX_BUFF_SIZE 256
#define CLI_MAX_PROCESSES 12

typedef struct _inbuff{
    char  buff[CLI_MAX_BUFF_SIZE];
    uint16_t in_count;
}inbuff_t;

class Cli;

typedef struct _process_unit{
    const char* name;
    void(*process_f)(Cli *cli);
    uint8_t used;
}process_unit_t;

class Cli{
  public:
    Cli();
    void init(HardwareSerial *serial, unsigned long baud);
    int  regProcess(void(*process_f)(Cli *cli), const char* name);
    void cliSerialEvent();
  private:
    int input(char in);
    int inputProcessing();
    int startProcess();
    void echo();
  public:
    
  private:
    process_unit_t process_units[CLI_MAX_PROCESSES];
    HardwareSerial *serial;
    inbuff_t inbuff;
};



#endif //CLI_H_
