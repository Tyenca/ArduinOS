#include "processSystem.hpp"


process processTable[10];
int noOfProcesses = 0;

// void pushByte(byte b) {
//   int SP = processTable[0].SP;
//   processTable[0].stack[SP] = b;
// }
// // int processID
// byte popByte() {
//   // return processTable[0].stack[--processTable[0].SP];
// }

int findProcess(int processID) {
  for (int i = 0; i < noOfProcesses; i++) {
    if (processTable[noOfProcesses].processID == processID && processTable[noOfProcesses].state != '0') {
      return i;
    }
  }
  return -1;  // process does not excist
}

void startProcess() {
  //Check if there is free space in process table
  if (noOfProcesses == MAXPROCESSES) {
    Serial.println("Max processes reached. No space left");
    return;
  }

  //Check if file exists
  // char filename[12]; // Assuming filename has enough space
  // strcpy(filename, parameter1);
  int FATindex = findFile(parameter1);
  if (FATindex == -1) {
    Serial.println(F("File does not exist."));
    return;
  }

  processTable[noOfProcesses].name = parameter1;
  processTable[noOfProcesses].state = RUNNING;
  processTable[noOfProcesses].processID = noOfProcesses;
  // processTable[noOfProcesses].PC = FAT[FATindex].addr;
  processTable[noOfProcesses].sp = 0;
  processTable[noOfProcesses].addr = 0;  // TO DO

  Serial.print(F("Process succesfully started. ID = "));
  Serial.print(processTable[noOfProcesses].processID);

  noOfProcesses++;
}

void changeState(int processIndex, char newState) {
  if (processTable[processIndex].state == newState) {
    Serial.print(F("Process is already in state"));
    Serial.println(newState);
  } else {
    processTable[processIndex].state == newState;
    Serial.print(F("Process state changed to "));
    Serial.println(newState);
  }
}
void suspendProcess() {
  int processIndex = findFile(parameter1);
  if (processIndex == -1) {
    Serial.println(F("Process not found"));
    return;
  }
  changeState(processIndex, PAUSED);
}
void resumeProcess() {
  int processIndex = findFile(parameter1);
  if (processIndex == -1) {
    Serial.println(F("Process not found"));
    return;
  }
  changeState(processIndex, RUNNING);
}
void killProcess() {
  int processIndex = findFile(parameter1);
  if (processIndex == -1) {
    Serial.println(F("Process not found"));
    return;
  }
  changeState(processIndex, TERMINATED);

  for (int i = processIndex; i < noOfProcesses; i++) {
    //rewrite entries
    processTable[i] = processTable[i + 1];
    Serial.println("Process terminated");
  }
  noOfProcesses--;
}
void listProcesses() {
  for (int i = 0; i < noOfProcesses; i++) {
    if (processTable[i].state != TERMINATED) {
      Serial.println(F("Process terminated"));
    }
  }
}


/////////////////////////////////////////////////
//Checkpoint 6
/////////////////////////////////////////////////

void runProcesses() {
  for (int i = 0; i < noOfProcesses; i++) {
    if (processTable[i].state == RUNNING) {
      // execute(findProcess(processTable[i].processID));
      Serial.print("");
      //Maak een functie die de process table doorloopt
      //en voor ieder proces met de toestand RUNNING een
      //instructie inleest en uitvoert
    }
  }
}


void execute(int processIndex) {

  int address = processTable[processIndex].fp;
  int instruction = 0;
  // pushBytes(processIndex, CHAR);
  switch (instruction) {
    case CHAR:
      //Point pc to next instruction
      pushChar(EEPROM.read(processTable[processIndex].fp), processIndex);
      processTable[processIndex].pc += 1;
      break;
    case INT:
      //Point pc to next instruction
      //FP OF PC??
      int high = EEPROM.read(address + processTable[processIndex].pc);
      processTable[processIndex].pc += 1;
      int low = EEPROM.read(address + processTable[processIndex].pc);
      pushInt(word(high, low), processTable[processIndex].processID);
      processTable[processIndex].pc += 1;
      break;
    case STRING:
      // processTable[processIndex].pc += 1;
      int length = EEPROM.read(processTable[processIndex].fp);  //Length
      char string[length];
      for (int i = 0; i < length + 1; i++) {
        string[i] = EEPROM.read(processTable[processIndex].pc);
        processTable[processIndex].pc += 1;
      }
      pushString(string, processTable[processIndex].processID);
      processTable[processIndex].pc += 1;
      break;
    case FLOAT:
      // processTable[processIndex].pc += 1;
      byte b[4];
      for (int i = 3; i >= 0; i--) {
        b[i] = EEPROM.read(address + processTable[processIndex].pc);
        processTable[processIndex].pc += 1;
      }
      float *pf = (float *)b;
      pushFloat(*pf, processTable[processIndex].processID);
      break;
    case PRINT:
      // peekType(processTable[processIndex].processID);
      printVar(processTable[processIndex].processID);
      break;
    case PRINTLN:
      printVar(processTable[processIndex].processID);
      Serial.println();
      break;
    case GET:
      readVariableFromMemoryTable(byte name, int processID);
      processTable[processIndex].programCounter += 1;
      break;
    case SET:
      saveVariableToMemoryTable(processTable[processIndex].name, processTable[processIndex].processID);
      processTable[processIndex].programCounter += 1;
      break;
  }
}
void printVar(int processID) {
  byte type = popByte(processID);

  switch (type) {
    case CHAR:
      Serial.print((char)popByte(processID));
      break;
    case INT:
      Serial.print(popInt(processID));
      break;
    case STRING:
      Serial.print(popString(processID));
      break;
    case FLOAT:
      Serial.print(popFloat(processID));
      break;
    default:
      Serial.println("Variable type not recognized");
      break;
  }
}
void pushBytes(int processIndex, int type) {

  for (int i = 0; i < type; i++) {
    processTable[processIndex].stack[processTable[processIndex].sp++] = EEPROM.read(processTable[processIndex].fp + i);
  }
}
