#ifndef CLI.hpp
#define CLI.hpp

#include <Arduino.h>

#define BUFSIZE 12

inline char inputBuffer[BUFSIZE];
inline char parameter1[BUFSIZE];
inline char parameter2[BUFSIZE];
inline char parameter3[BUFSIZE];
inline char data[BUFSIZE];

static int charPos; // static variables default to 0
static int flag;
static bool isCommand;

void store();
void retrieve();
void erase();
void files();
void freespace();
void run();
void list();
void suspend();
void resume();
void kill();
void test();

bool commandFound();
void callFunction();
int allParametersRequired();
void commandLine();
void clearBuffers();

typedef struct {
  char name [ BUFSIZE ];
  void (* func )();
  int parameters;
} commandType ;

static commandType command [] = {
  {"store", &store, 3 },
  {"retrieve", &retrieve, 1},
  {"erase", &erase, 1},
  {"files", &files, 0},
  {"freespace", &freespace, 0},
  {"run", &run, 1},
  {"list", &list, 0},
  {"suspend", &suspend, 1},
  {"resume", &resume, 1},
  {"kill", &kill, 1},
  {"test", &test, 1}
};

#endif
