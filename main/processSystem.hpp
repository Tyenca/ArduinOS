#ifndef processSystem.hpp
#define processSystem.hpp

#include <Arduino.h>
#include "instruction_set.h"
#include "cli.hpp"
#include "fileSystem.hpp"
#include "memorySystem.hpp"

#define RUNNING 'r' 
#define PAUSED 'p' 
#define TERMINATED '0' 

#define MAXPROCESSES 10
#define STACKSIZE 32

void runProcess();
void suspendProcess();
void killProcess();
void listProcesses();
// void pushByte(byte b);
// byte popByte();
// void pushBytes(int processIndex, int type);


// int findProcess();

struct process {
  byte name;              //filename
  char state;             //RUNNING ('r'), PAUSED ('p') or TERMINATED (0)
  int processID;  
  int pc;                 //program counter 
  int fp;                 //file pointer(address)
  int sp;                 //stack pointer
  int addr;
  byte stack[STACKSIZE];  //seperate stack for each process
};

extern process processTable[10];

#endif