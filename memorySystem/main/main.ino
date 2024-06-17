#include "cli.hpp"
#include "memorySystem.hpp"
#include <EEPROM.h>


void commandLine();
void clearMemory();

void setup() {
  // put your setup code here, to run once:
  clearMemory();
  Serial.begin(9600);
  // inputBuffer[0] = '\0';
  Serial.println(F("ArduinOS 1.0 ready"));

  // testCharVars();
  // testIntVars();
  // testFloatVars();
  // testStringVars();

  // testSaveVars();
  // testReadVars();

}

void loop() {
  // put your main code here, to run repeatedly:
  // commandLine();
  // pushInt(10, 0);
  // popByte(0); //type
  // popInt(0);
  
}

void clearMemory() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

void testCharVars() {
  Serial.println("-------------CHAR TEST-------------");
  pushChar('x', 0);
  popByte(0); //type
  popChar(0);

  pushChar('a', 0);
  popByte(0); //type
  popChar(0);
}

void testIntVars() {
  Serial.println("-------------INT TEST-------------");
  pushInt(10, 0);
  popByte(0); //type
  popInt(0);

  pushInt(100, 0);
  popByte(0); //type
  popInt(0);
}

void testStringVars() {
  Serial.println("-------------STRING TEST-------------");
  pushString("hallo", 0);
  popByte(0); //type
  popString(0);

  pushString("ArduinOS 1.0 ready", 0);
  popByte(0); //type
  popString(0);
}


void testFloatVars() {
  Serial.println("-------------FLOAT TEST-------------");
  pushFloat(10.5, 0);
  popByte(0); //type
  popFloat(0);

  pushFloat(123.54, 0);
  popByte(0); //type
  popFloat(0);
}

void testSaveVars() {
  pushChar('x', 0);
  saveVariableToMemoryTable('testChar', 0);
  
  pushInt(100, 1);
  saveVariableToMemoryTable('testInt', 1);

  //Overwrite existing entry
  pushChar('a', 0);
  saveVariableToMemoryTable('testChar', 0);
}

void testReadVars() {
  pushChar(10, 0);
  saveVariableToMemoryTable('testChar', 0);
  readVariableFromMemoryTable('testChar', 0);

  pushInt(100, 1);
  saveVariableToMemoryTable('testInt', 1);
  readVariableFromMemoryTable('testInt', 1);
}