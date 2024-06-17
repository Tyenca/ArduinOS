#include "memorySystem.hpp"

//STACK
byte stack[STACKSIZE];
byte sp = 0;
void pushByte(byte b, int processID) {
  stack[sp++] = b;
  // Serial.println(b);
}
byte popByte(int processID) {
  byte popped = stack[--sp];
  // Serial.println(popped);
  return popped;
}
// void pushByte(byte b, int processID) {
//   processTable[processID].stack[processTable[processID].sp++] = b;
// }

// byte popByte(int processID) {
// return processTable[processID].stack[--processTable[processID].sp];
// }

variable memoryTable[MAXVARIABLES];
byte MEMORY[256];  //RAM memory

int noOfVars = 0;
//PUSH
void pushChar(char value, int processID) {
  pushByte(value, processID);
  pushByte(CHAR, processID);

  Serial.print("value pushed: ");
  Serial.println(value);
}
void pushInt(int value, int processID) {
  pushByte(highByte(value), processID);
  pushByte(lowByte(value), processID);
  pushByte(INT, processID);

  Serial.print("value pushed: ");
  Serial.println(value);
}
void pushFloat(float value, int processID) {
  byte b[4];
  float *pf = (float *)b;
  *pf = value;
  for (int i = 3; i >= 0; i--) {
    pushByte(b[i], processID);
    // Serial.println(b[i]);
  }
  pushByte(FLOAT, processID);
  Serial.print("value pushed: ");
  Serial.println(value);
}
void pushString(char string[], int processID) {
  for (int i = 0; i < strlen(string); i++) {
    pushByte(string[i], processID);
    // Serial.println(string[i]);
  }
  pushByte('\0', processID);                //Push terminating zero
  pushByte(strlen(string) + 1, processID);  //Push length string + terminating zero
  pushByte(STRING, processID);

  Serial.print("String pushed: ");
  Serial.println(string);
}
//NOG NIET AF
char popString(int processID) {
  int length = popByte(processID);  
  Serial.print("Popped length: ");
  Serial.println(length);

  // Allocate memory for the string 
  char* string = new char[length];

  for (int i = length - 1; i >= 0; i--) {
    string[i] = popByte(processID);
  }

  Serial.print("value popped: ");
  Serial.print(string); 
  return string;
}
//POP
int popInt(int processID) {
  // popByte() //Type
  int LowByte = popByte(processID);
  int highByte = popByte(processID);

  Serial.print("value popped: ");
  Serial.println(word(highByte, LowByte));
  return word(highByte, LowByte);  //word(highByte, LowByte)
}
float popFloat(int processID) {
  // popByte() //Type
  byte b[4];
  for (int i = 0; i < 4; i++) {
    b[i] = popByte(processID);  //popByte(b[i]);
      // Serial.println(b[i]);
  }
  float *pf = (float *)b;
  Serial.print("value popped: ");
  Serial.println(*pf);
  return *pf;
}
char popChar(int processID) {
   // popByte() //Type
  char popped = stack[--sp];

  Serial.print("value popped: ");
  Serial.println(popped);

  return popped;
}

float popVal(int processID) {
  int type = popByte(processID);
  if (type == 1) {
    return popChar(processID);
  } else if (type == 2) {
    return popInt(processID);
  } else if (type == 3) {
    return popString(processID);
  } else if (type == 4) {
    return popFloat(processID);
  } else {
    Serial.println("Type operator not recognized");
  }
}

// Return top stack
/*byte peekType(int processID) {
  int byte = popByte(processID);
  if (CHAR) {
    Serial.println("Type on stack is CHAR");
    pushByte(CHAR, processID);
  } else if (INT) {
    Serial.println("Type on stack is INT");
    pushByte(INT, processID);
  } else if (STRING) {
    Serial.println("Type on stack is STRING");
    pushByte(INT, processID);
  } else if (FLOAT) {
    Serial.println("Type on stack is FLOAT");
    pushByte(FLOAT, processID);
  } else {
    Serial.println("Type operator not recognized");
    pushByte(byte, processID);
  }
  return byte;
}*/
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
    // Serial.println(F("FILES SORTED"));
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
    int freeSpaceStart = memoryTable[i].addr + memoryTable[i].size;
    int freeSpaceEnd = memoryTable[i + 1].addr;
    int freeSpace = freeSpaceEnd - freeSpaceStart;

    if (freeSpace >= size) {
      Serial.println("findFreeSpaceInMemory(): check space between blocks");
      return freeSpaceStart;
    }
  }
  //check space after last block
  int lastBlockEnd = (memoryTable[noOfVars - 1].addr + memoryTable[noOfVars - 1].size) + 1;
  // Serial.println(lastBlockEnd);
  if (MEMORY - lastBlockEnd >= size) {
    // Serial.println("findFreeSpaceInMemory(): check space after last block");
    return lastBlockEnd ;
  }

  return -1;
}
void saveVariableToMemoryTable(byte name, int processID) {

  /*if (noOfVars == 1) {
    Serial.print("-------------TEST BLOCK-----------");
    Serial.print("Name:\t");
    Serial.println(memoryTable[0].name);
    Serial.print("Type:\t");
    Serial.println(memoryTable[0].type);
    Serial.print("Addr:\t");
    Serial.println(memoryTable[0].addr);
    Serial.print("Size:\t");
    Serial.println(memoryTable[0].size);
    Serial.print("ID:\t");
    Serial.println(memoryTable[0].processID);
    Serial.print("--------------------------------");
  }
  if (noOfVars == 3) {
    Serial.print("-------------TEST BLOCK-----------");
    for (int i = 0; i < noOfVars - 1; i++) {
    Serial.print("Name:\t");
      Serial.println(memoryTable[noOfVars].name);
    }
    Serial.println(memoryTable[0].name);
    Serial.print("Type:\t");
    Serial.println(memoryTable[0].type);
    Serial.print("Addr:\t");
    Serial.println(memoryTable[0].addr);
    Serial.print("Size:\t");
    Serial.println(memoryTable[0].size);
    Serial.print("ID:\t");
    Serial.println(memoryTable[0].processID);
    Serial.print("--------------------------------");
  }*/
  
  // Check if there is already a variable with this name
  //if(findVariable(/*NAAM*/)  !== -1)
  for (int i = 0; i < noOfVars ; i++) {
    Serial.print("NAME: ");
    Serial.println(memoryTable[i].name);
    if (strcmp(memoryTable[i].name, name) == 0 && memoryTable[i].processID == processID) {
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
    size = CHAR;
  } else if (type == INT) {
    size = INT;
  } else if (type == STRING) {
    size = popByte(processID);
    // pushByte(size, processID);  //put size back on stack
  } else if (type == FLOAT) {
    size = FLOAT;
  }

  int address = findFreeSpaceInMemoryTable(size);

  if (address == -1) {
    return;
  }
  
  // Create new entry in memory table
  memoryTable[noOfVars].name = name;
  memoryTable[noOfVars].type = type;
  memoryTable[noOfVars].addr = address;
  memoryTable[noOfVars].size = size;
  memoryTable[noOfVars].processID = processID;

  // Serial.print("Name:\t");
  // Serial.println(memoryTable[noOfVars].name);
  // Serial.print("Type:\t");
  // Serial.println(memoryTable[noOfVars].type);
  // Serial.print("Addr:\t");
  // Serial.println(memoryTable[noOfVars].addr);
  // Serial.print("Size:\t");
  // Serial.println(memoryTable[noOfVars].size);
  // Serial.print("ID:\t");
  // Serial.println(memoryTable[noOfVars].processID);
  Serial.println("File stored");

  //Write data to memory address
  // Reverse order
  if (type == STRING) {
    for (int i = size - 1; i >= 0; i--) {
      MEMORY[address] = popByte(processID);
      address++;
    	}
  } 
  else {
    for (int i = 0; i < size; i++) {
      byte b = popByte(processID);
      MEMORY[address] = b;
      address++;
      Serial.print("pushed:\t");
      Serial.println(b);
    }
  }
  
  noOfVars++;
  Serial.print("nr of vars: ");
  Serial.println(noOfVars);
}
void readVariableFromMemoryTable(byte name, int processID) {
  // Search for variable in memoryTable
  int address = -1;
  int size;
  byte type;
  for (int i = 0; i < noOfVars; i++) {
    if (memoryTable[i].processID == processID) {
      address = memoryTable[i].addr;
      Serial.print("address: ");
      Serial.println(address);
      size = memoryTable[i].size;
      type = memoryTable[i].type;
      break;
    }
  }
  if (address == -1) {
    Serial.println(F("Variable not found."));
    return;
  }
  // Push bytes to stack
  if (type == CHAR) {
    for (int i = 0; i < size; i++) {
      byte b = MEMORY[address + i];
      pushByte(b, processID);
      Serial.print("popped:\t");
      Serial.println(b);
    }
    pushByte(CHAR, processID);
  }
  if (type == INT) {
    for (int i = 0; i < size; i++) {
      byte b = MEMORY[address + i];
      pushByte(b, processID);
      Serial.print("popped:\t");
      Serial.println(b);
    }
    pushByte(INT, processID);
  }
  if (type == STRING) {
    for (int i = 0; i < size; i++) {
      pushByte(MEMORY[address + i], processID);
    }
    pushByte(size, processID);
    pushByte(STRING, processID);
  }
  if (type == FLOAT) {
    for (int i = 0; i < size; i++) {
      pushByte(MEMORY[address + i], processID);
    }
    pushByte(FLOAT, processID);
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
      Serial.print("Deleten all processes with ID: ");
      Serial.print(processID);
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
