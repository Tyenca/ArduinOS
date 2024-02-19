#include "cli.hpp"
#include <EEPROM.h>
// #include "fileSystem.hpp"

void commandLine();
void clearMemory();

void setup() {
  // put your setup code here, to run once:
 clearMemory();
  Serial.begin(9600);
  //  inputBuffer[0] = '\0';
  Serial.println(F("ArduinOS 1.0 ready"));
}

void loop() {
  // put your main code here, to run repeatedly:
  commandLine();
}

void clearMemory() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}
