#ifndef memorySystem.hpp
#define memorySystem.hpp

#include <Arduino.h>
#include "cli.hpp"

#define CHAR 1    // 1-byte // plaats op stack (eerst de waarde, dan het type)
#define INT 2     // 2-byte // plaats op stack (eerst de waarde, dan het type)
#define STRING 3  // n-byte (zero-terminated) // plaats op stack (eerst de string, dan de lengte (byte), dan het type)
#define FLOAT 4   // 4-byte // plaats op stack (eerst de waarde, dan het type)

#define MAXPROCESSES 25
#define STACKSIZE 32

// void saveVariableToMemoryTable();
void testFunc();

#endif
