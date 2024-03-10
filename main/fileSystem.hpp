#ifndef fileSystem.hpp
#define fileSystem.hpp

#include <Arduino.h>
#include <EEPROM.h>
#include "cli.hpp"

#define MAXFILES 10
#define FATSIZE 256

extern EERef noOfFiles;

//struct FATEntry {
//  char filename[12]; //12 Bytes
//  int startPosition; //2 Bytes
//  int length; //2 Bytes
//};

//class EERef {
//
//};
//
//EERef noOfFiles;

//void writeFATEntry(FATEntry entry);
//void readFATEntry();

// File allocation table: arrays of names, starting addresses and file lengths

struct FATEntry {
  char filename[12];
  int addr;
  int filesize;
};

extern FATEntry FAT[MAXFILES];

void storeFile();
void retrieveFile();
void eraseFile();
void showFiles();
void checkFreeSpace();
int findFile(char name[BUFSIZE]);

#endif
