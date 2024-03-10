#include "memorySystem.hpp"

//STACK
// byte stack[STACKSIZE];
// byte sp = 0;
// void pushByte(byte b) {
//   stack[sp++] = b;
// }
// byte popByte() {
//   return stack[--sp];
// }
// , int processID
void pushByte(byte b, int processID) {
  processTable[processID].stack[processTable[processID].sp++] = b;
}
// int processID
byte popByte(int processID) {
  return processTable[processID].stack[--processTable[processID].sp];
}

struct variable {
  byte name;
  byte type;  //KLOPT DIT?
  int addr;
  int size;
  int processID;
};

int noOfVars = 0;
variable memoryTable[MAXVARIABLES];
byte MEMORY[256];  //RAM memory

//PUSH
void pushChar(char value, int processID) {
  pushByte(value, processID);
  pushByte(CHAR, processID);
}
void pushInt(int value, int processID) {
  pushByte(highByte(value), processID);
  pushByte(lowByte(value), processID);
  pushByte(INT, processID);
}
void pushFloat(float value, int processID) {
  byte b[4];
  float *pf = (float *)b;
  *pf = value;
  for (int i = 3; i >= 0; i--) {
    pushByte(b[i], processID);
  }
  pushByte(FLOAT, processID);
}
void pushString(char string[], int processID) {
  for (int i = 0; i < sizeof(string); i++) {
    pushByte(string[i], processID);
  }
  pushByte('\0', processID);                //Push terminating zero
  pushByte(sizeof(string) + 1, processID);  //Push length string + terminating zero
  pushByte(STRING, processID);
}
//NOG NIET AF
char popString(int processID) {
  // popByte(processID);               //type
  int length = popByte(processID);  //Length
  char string[length];
  for (int i = 0; i < length + 1; i++) {
    string[i] =+ popByte(processID);
  }
  return string;
}
//POP
int popInt(int processID) {
  // popByte() //Type
  int LowByte = popByte(processID);
  int highByte = popByte(processID);
  return word(highByte, LowByte);  //word(highByte, LowByte)
}
float popFloat(int processID) {
  // popByte() //Type
  byte b[4];
  for (int i = 3; i >= 0; i--) {
    b[i] = popByte(processID);  //popByte(b[i]);
  }
  float *pf = (float *)b;
  return *pf;
}
// char popChar() {
////identiek aan popByte()
// }

float popVal(int processID) {
  int type = popByte(processID);
  if (type == 1) {
    return popByte(processID);
  } else if (type == 2) {
    return popInt(processID);
  } else if (type == 4) {
    return popFloat(processID);
  }
}

// Return top stack
byte peekType(int processID) {
  int byte = popByte(processID);
  // char type[] = "";
  if (CHAR) {
    Serial.print("Type on stack is CHAR");
    pushByte(CHAR, processID);
  } else if (INT) {
    Serial.print("Type on stack is INT");
    pushByte(INT, processID);
  } else if (FLOAT) {
    Serial.print("Type on stack is FLOAT");
    pushByte(FLOAT, processID);
  }
  else {
    Serial.print("Type unknown");
    pushByte(byte, processID);
  }

  return byte;
}

int findVariable(char name[BUFSIZE]) {
  for (int i = 0; i < noOfVars; i++) {
    if (strcmp(memoryTable[i].name, name) == 0) {
      return i;
    }
  }
  return -1;  // file does not excist
}
void sortVariables() {
  //  FATEntry entry;
  bool sorted = false;
  while (!sorted) {
    for (int i = 0; i < noOfVars - 1; i++) {
      if (memoryTable[i].addr > memoryTable[i + 1].addr) {
        // Swap FATEntries
        variable temp = memoryTable[i];
        memoryTable[i] = memoryTable[i + 1];
        memoryTable[i + 1] = temp;
        sorted = false;
      }
    }
    sorted = true;
    Serial.println(F("FILES SORTED"));
  }
}
int findFreeSpaceInMemoryTable(byte size) {
  sortVariables();

  if (noOfVars == MAXVARIABLES) {
    Serial.println("findFreeSpaceInMemory(): max variables reached");
    return -1;  //not enough space left
  }
  if (noOfVars == 0) {
    return 0;  //Return first usable address
  }

  //check space between blocks
  for (int i = 0; i < noOfVars - 1; i++) {
    int freeSpace = memoryTable[i + 1].addr - (memoryTable[i].addr + memoryTable[i].size);
    if (freeSpace >= size) {
      return memoryTable[i].addr + memoryTable[i].size;
    }
  }
  //check space after last block
  if (MEMORY - (memoryTable[noOfVars - 1].addr + memoryTable[noOfVars - 1].size) >= size) {
    return MEMORY - (memoryTable[noOfVars - 1].addr + memoryTable[noOfVars - 1].size);
  }

  return -1;
}
void saveVariableToMemoryTable(byte name, int processID) {
  // int address = findFreeSpaceInMemoryTable(name);  // SIZE MEEGEVEN
  // // peek func gebruiken?

  // if (address == -1) {
  //   Serial.println("No space left");
  //   return;
  // }

  // Check if there is already a variable with this name
  //if(findVariable(/*NAAM*/)  !== -1)
  for (int i = 0; i < noOfVars; i++) {
    if (strcmp(memoryTable[noOfVars].name, name) == 0 && memoryTable[noOfVars].processID == processID) {
      //rewrite entries
      for (int j = i; j < noOfVars - 1; j++) {
        memoryTable[j] = memoryTable[j + 1];
      }
      Serial.println("Existing entry with same name & processID is overwritten");
      noOfVars--;
    }
  }

  sortVariables();
  byte type = popByte(processID);
  int size = 0;
  if (type == CHAR) {
    size = 1;
  } else if (type == INT) {
    size = 2;
  } else if (type == STRING) {
    size = popByte(processID);
    pushByte(size, processID); //put size back on stack
  } else if (type == FLOAT) {
    size = 4;
  }

  int address = findFreeSpaceInMemoryTable(size);

  if (address == -1) {
    Serial.println("No space left");
    return;
  }

  // Create new entry in memory table
  memoryTable[noOfVars].name = name;
  memoryTable[noOfVars].type = type;
  memoryTable[noOfVars].addr = address;
  memoryTable[noOfVars].size = size;
  memoryTable[noOfVars].processID = processID;

  if (type == STRING) {
    for (int i = size - 1; i >= 0; i--) {
      MEMORY[address] = popByte(processID);
      address++;
    }
  } else {
    for (int i = 0; i <= type; i++) {
      MEMORY[address] = popByte(processID);
      address++;
    }
  }

  noOfVars++;
}
void readVariableFromMemoryTable(byte name, int processID) {
  // int fileIndex = findFile(parameter1);
  // Search for variable in memoryTable
  int address = -1;
  int size = 0;
  variable process;
  for (int i = 0; i < noOfVars; i++) {
    if (memoryTable[noOfVars].processID == processID) {
      process = memoryTable[noOfVars];
      address = process.addr;
      size = process.size;
      break;
    }
  }
  if (address == -1) {
    Serial.println(F("Variable not found."));
    return;
  }

  // Push bytes to stack
  if (process.type == CHAR) {
    for (int i = 0; i < process.size; i++) {
      pushByte(MEMORY[process.addr], processID);
    }
    pushByte(CHAR, processID);
  }
  if (process.type == INT) {
    for (int i = 0; i < process.size; i++) {
      pushByte(MEMORY[process.addr], processID);
    }
    pushByte(INT, processID);
  }
  if (process.type == STRING) {
    for (int i = 0; i < process.size; i++) {
      pushByte(MEMORY[process.addr], processID);
    }
    pushByte(size, processID);
    pushByte(STRING, processID);
  }
  if (process.type == FLOAT) {
    for (int i = 0; i < process.size; i++) {
      pushByte(MEMORY[process.addr], processID);
    }
    pushByte(INT, processID);
  }
}
void deleteAllVariables(int processID) {

  for (int i = 0; i < noOfVars; i++) {
    if (memoryTable[noOfVars].processID == processID) {
      //rewrite variables
      for (int j = i; j < noOfVars - 1; j++) {
        memoryTable[j] = memoryTable[j + 1];
      }    
      noOfVars--;

      Serial.print("All process with ");
      Serial.print(processID);
      Serial.println(" deleted");  
    }
  }
}








// void testFunc(int processIndex) {
//   if (strcmp(parameter1, "push") == 0) {
//     Serial.println("byte pushed");
//     pushByte(FLOAT, processIndex);
//   }
//   if (strcmp(parameter1, "pop") == 0) {
//     // Serial.println("get gedetecteerd");
//     Serial.println(popByte(processIndex));
//     //OUTPUT: 4
//   }
// }

// static Variable memoryTable[MAXVARIABLES];