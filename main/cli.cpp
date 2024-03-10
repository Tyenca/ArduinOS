#include <string.h>
#include "cli.hpp"
#include "fileSystem.hpp"
#include "memorySystem.hpp"

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
void commandLine() {
  // put your main code here, to run repeatedly:
  while (Serial.available() > 0) {
    char input = Serial.read();
    input = tolower(input);
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
      if(charPos > BUFSIZE) {
         Serial.println(F("Too many characters, start over"));
         charPos = 0;
         clearBuffers();
         return;
      }

      isCommand = commandFound();
      if (isCommand == false) {
        Serial.println(F("Command is not recognized"));
        memset(inputBuffer, 0, sizeof(inputBuffer));
      }
      if (isCommand) {
        flag++;
      }
      if (flag == allParametersRequired()) {
        callFunction();
      }
      charPos = 0;
    }
  }
}
void clearBuffers() {
  memset(inputBuffer, 0, sizeof(inputBuffer));
  memset(parameter1, 0, sizeof(inputBuffer));
  memset(parameter2, 0, sizeof(inputBuffer));
  memset(parameter3, 0, sizeof(inputBuffer));
  
  flag = 0;
  isCommand = false;
}
void store() {
//  Serial.println(F("subroutine store aangeroepen in cli: "));
  strncpy(data, parameter3, atoi(parameter2)); //removes the characters that are bigger than the given size
  storeFile();
  clearBuffers();
}
void retrieve() {
//  Serial.println(F("subroutine retrieve aangeroepen: "));
  retrieveFile();
  clearBuffers();
}
void erase() {
//  Serial.println(F("subroutine erase aangeroepen: "));
  eraseFile();
  clearBuffers();
}
void files() {
  showFiles();
//  Serial.println(F("subroutine files aangeroepen: "));
  clearBuffers();
}
void freespace() {
  checkFreeSpace();
//  EEPROM.length()
//  Serial.println(F("subroutine freespace aangeroepen: "));
  clearBuffers();
}
void run() {
  Serial.println(F("subroutine run aangeroepen: "));
  clearBuffers();
}
void list() {
  Serial.println(F("subroutine list aangeroepen: "));
  clearBuffers();
}
void suspend() {
  Serial.println(F("subroutine suspend aangeroepen: "));
  clearBuffers();
}
void resume() {
  Serial.println(F("subroutine resume aangeroepen: "));
  clearBuffers();
}
void kill() {
  Serial.println(F("subroutine kill aangeroepen: "));
  clearBuffers();
}
void test() {
  // testFunc();
  Serial.println(F("subroutine TEST aangeroepen: "));
  clearBuffers();
}
