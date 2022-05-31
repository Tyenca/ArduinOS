#include <EEPROM.h>
#include <string.h>

#define BUFSIZE 12
char inputBuffer[BUFSIZE];
char parameter1[BUFSIZE];
char parameter2[BUFSIZE];
char parameter3[BUFSIZE];
static int charPos; // static variables default to 0
static int flag;
static bool isCommand = false;

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
  {"kill", &kill, 1}
};

static int commandTypeSize = sizeof(command) / sizeof(commandType);

bool commandFound() {
  for (int i = 0; i < commandTypeSize; i++) {
    if (strcmp(command[i].name, inputBuffer) == 0) {
      return true;
    }
  }
  return false;
}
void callFunction() {
  for (int i = 0; i < commandTypeSize; i++) {
    if (strcmp(command[i].name, inputBuffer) == 0) {
      // call the matched function
      command[i].func();
    }
  }
}
int allParametersRequired() {
  for (int i = 0; i < commandTypeSize; i++) {
    if (strcmp(command[i].name, inputBuffer) == 0) {
      return command[i].parameters + 1;
    }
  }
  return 0;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  inputBuffer[0] = '\0';
  Serial.println(F("ArduinOS 1.0 ready"));
}
void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available() > 0) {
    char input = Serial.read();
    if (input != '\n' && input != '\r' && isCommand == false) {
      inputBuffer[charPos++] = input;
    }
    else if (input != '\n' && input != '\r' && isCommand == true) {
      switch (flag) {
        case 1:
          parameter1[charPos++] = input;
          break;
        case 2:
          parameter2[charPos++] = input;
          break;
        case 3:
          parameter3[charPos++] = input;
          break;
      }
    }
    else {
      isCommand = commandFound();
      if (isCommand == false) {
        Serial.println(F("Command is not recognized"));
      }
      if (isCommand) {
        flag++;
      }
      if (flag == allParametersRequired()) {
        flag = 0;
        callFunction();
      }
      charPos = 0;
    }
  }
}
void store() {
  clearBuffers();
  Serial.println(F("subroutine store aangeroepen: "));
}
void retrieve() {
  clearBuffers();
  Serial.println(F("subroutine retrieve aangeroepen: "));
}
void erase() {
  clearBuffers();
  Serial.println(F("subroutine erase aangeroepen: "));
}
void files() {
  clearBuffers();
  Serial.println(F("subroutine files aangeroepen: "));
}
void freespace() {
  clearBuffers();
  Serial.println(F("subroutine freespace aangeroepen: "));
}
void run() {
  clearBuffers();
  Serial.println(F("subroutine run aangeroepen: "));
}
void list() {
  clearBuffers();
  Serial.println(F("subroutine list aangeroepen: "));
}
void suspend() {
  clearBuffers();
  Serial.println(F("subroutine suspend aangeroepen: "));
}
void resume() {
  clearBuffers();
  Serial.println(F("subroutine resume aangeroepen: "));
}
void kill() {
  clearBuffers();
  Serial.println(F("subroutine kill aangeroepen: "));
}
void clearBuffers() {
  memset(inputBuffer, 0, sizeof(inputBuffer));
  memset(parameter1, 0, sizeof(inputBuffer));
  memset(parameter2, 0, sizeof(inputBuffer));
  memset(parameter3, 0, sizeof(inputBuffer));
  
  isCommand = false;
}
