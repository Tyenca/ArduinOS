#ifndef memorySystem.hpp
#define memorySystem.hpp

#include <Arduino.h>
#include "cli.hpp"
// #include "processSystem.hpp"

#define CHAR 1    // 1-byte // plaats op stack (eerst de waarde, dan het type)
#define INT 2     // 2-byte // plaats op stack (eerst de waarde, dan het type)
#define STRING 3  // n-byte (zero-terminated) // plaats op stack (eerst de string, dan de lengte (byte), dan het type)
#define FLOAT 4   // 4-byte // plaats op stack (eerst de waarde, dan het type)

#define MAXVARIABLES 25
#define STACKSIZE 32

struct variable {
  byte name;
  byte type;  //KLOPT DIT?
  int addr;
  int size;
  int processID;
};

byte popByte(int processID);
char popChar(int processID);
int popInt(int processID);
float popFloat(int processID);
char popString(int processID);
float popVal(int processID);

void pushByte(byte b, int processID);
void pushChar(char value, int processID);
void pushInt(int value, int processID);
void pushFloat(float value, int processID); 
void pushString(char string[], int processID);

byte peekType(int processID);

void saveVariableToMemoryTable(byte name, int processID);
void readVariableFromMemoryTable(byte name, int processID);

// void saveVariableToMemoryTable();
void testFunc();

#endif
