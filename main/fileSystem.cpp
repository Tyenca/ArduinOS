#include "fileSystem.hpp"
//#include "tinbes.h"
//int firstUseableAdd = 161;
//int freeSpace = EEPROM.length() - 161 ;

EERef noOfFiles = EEPROM[160];  // 1 byte - extern EERef noOfFiles = EEPROM[160]
int firstUseableAddress = 161;
//int numberOfFiles = 0;


#define TABLE_SIZE 12

// File allocation table: arrays of names, starting addresses and file lengths

FATEntry FAT[MAXFILES];

void writeFATEntry(FATEntry entry) {
  FAT[noOfFiles] = entry;
  int address = 16 * noOfFiles;
  EEPROM.put(address, FAT[noOfFiles]);
  noOfFiles++;
}
FATEntry readFATEntry(int FATIndex, FATEntry *entry) {
  int address = 16 * FATIndex;
  EEPROM.get(address, *entry);
  return *entry;
}

// Sort the file allocation table by starting address using bubble sort
void sort() {
  //  FATEntry entry;
  bool sorted = false;
  while (!sorted) {
    for (int i = 0; i < noOfFiles - 1; i++) {
      if (FAT[i].addr > FAT[i + 1].addr) {
        // Swap FATEntries
        FATEntry temp = FAT[i];
        FAT[i] = FAT[i + 1];
        FAT[i + 1] = temp;
        sorted = false;
      }
    }
    sorted = true;
    Serial.println(F("FILES SORTED"));
  }
}
// Look for an unused block of disk space of at least fileSize and return starting address
int findFreeSpace() {
  sort();
  int fileSizeNewEntry = atoi(parameter2);

  if (noOfFiles == 10) {
    Serial.println("Max files reached. No space left");
    return -1;  //not enough space left
  }
  if (noOfFiles == 0) {
    return firstUseableAddress;
  }
  //check space after last block
  if ((EEPROM.length() - (FAT[noOfFiles - 1].addr + FAT[noOfFiles - 1].filesize)) > fileSizeNewEntry) {
    Serial.print("FREESPACE: ");
    Serial.println((EEPROM.length() - (FAT[noOfFiles - 1].addr + FAT[noOfFiles - 1].filesize)));
    return (FAT[noOfFiles - 1].addr + FAT[noOfFiles - 1].filesize);
  }

  return -1;
}


// Return index in FAT of file with fileName
int findFile(char name[BUFSIZE]) {
  FATEntry entry;
  for (int i = 0; i < noOfFiles; i++) {
    readFATEntry(i, &entry);
    if (strcmp(entry.filename, name) == 0) {
      return i;
    }
  }
  return -1;  // file does not excist
}
void storeFile() {
  int address = findFreeSpace();
  if (address == -1 || noOfFiles == TABLE_SIZE) {
    Serial.println(F("Not enough space."));
  } else if (findFile(parameter1) != -1) {
    Serial.println(F("File already exists."));
  } else {  // create new entry in FAT
    // TO DO
    FATEntry entry;

    strcpy(entry.filename, parameter1);
    entry.addr = address;
    entry.filesize = atoi(parameter2);

    writeFATEntry(entry);

    for (int i = 0; i <= entry.filesize; i++) {
      address++;
      char c = parameter3[i];
      EEPROM.write(address, c);
    }

    Serial.print("Filename:\t");
    Serial.println(entry.filename);
    Serial.print("Size:\t\t");
    Serial.println(entry.filesize);
    Serial.println("File stored");
    Serial.print("Address:\t");
    Serial.println(entry.addr);
    Serial.print("nr of files: ");
    Serial.println(noOfFiles);
  }
}
void retrieveFile() {
  //  Serial.print(F("Retrieving file "));
  //  Serial.println(parameter1);
  int fileIndex = findFile(parameter1);
  if (fileIndex == -1) {
    Serial.print(F("File not found.\n"));
    return;
  }
  //  String ret;
  FATEntry entry;
  //    for (long a = file0.addr[f]; a < file.addr[f] + file.filesize[f]; a++) {

  // TO DO
  readFATEntry(fileIndex, &entry);
  Serial.print(F("File retrieved: \t"));
  Serial.println(entry.filename);
  char character[entry.filesize];
  character[0] = '\0';
  int address = entry.addr ;
  for (int i = 0; i < entry.filesize + 1; i++) {
    character[i] = EEPROM.read(address++);
    Serial.print(character[i]);
  }
  //  return;
}

void eraseFile() {
  int fileIndex = findFile(parameter1);
  if (fileIndex == -1) {
    Serial.print(F("File not found.\n"));
    return;
  }
  //  FATEntry entry;
  //    readFATEntry(fileIndex, &entry)
  else {
    // overide the old element
    for (int i = fileIndex; i < noOfFiles; ++i) {
      FAT[i] = FAT[i + 1];  // copy next element left
      int address = 16 * i;
      EEPROM.put(address, FAT[i]);
    }
    //    FatEntry entry = {0, 0, 0};
    //    FAT[noOfFiles - 1] = file;

    noOfFiles--;
    //  sort();
  }
  Serial.print(F("File erased: \t"));
  Serial.println(parameter1);
}

void showFiles() {
  if (noOfFiles == 0) {
    Serial.println(F("No files in memory"));
    return;
  }
  FATEntry entry;
  Serial.println(F("Showing all files"));
  Serial.print(F("Number of files: \t"));
  Serial.println(noOfFiles);
  for (int i = 0; i < noOfFiles; i++) {
    readFATEntry(i, &entry);
    Serial.print(F("Filename: \t"));
    Serial.println(entry.filename);
    Serial.print(F("File size: \t"));
    Serial.println(entry.filesize);
    Serial.print(F("File address: \t"));
    Serial.println(entry.addr);
    Serial.println();
  }
}
void checkFreeSpace() {
  sort();
  // int systemMemory = (16 * noOfFiles) + 1;  // the space the FAT table takes up
  // int totalSpaceAvailable = 0;

  // add the space between the last file and the end of the filesystem
  int totalSpaceAvailable = (EEPROM.length() - (FAT[noOfFiles - 1].addr + FAT[noOfFiles - 1].filesize));

  Serial.print("Total space avaiable: ");
  Serial.println(totalSpaceAvailable);
}
/*

  bool fileNameIsAvailable(char name[BUFSIZE]) {
  FATEntry entry;
  for (int i = 0; i < noOfFiles; i++) {
    readFATEntry(i, &entry);
    if (strcmp(entry.filename, name) == 0) {
      return false;
    }
  }
  return true;
  }
  bool fileExists(char name[BUFSIZE]) {
  FATEntry entry;
  for (int i = 0; i < noOfFiles; i++) {
    readFATEntry(i, &entry);
    if (strcmp(entry.filename, name) == 0) {
      return true;
    }
  }
  return false;
  }
  int entryIndex(char name[BUFSIZE]) {
  FATEntry entry;
  for (int i = 0; i < noOfFiles; i++) {
    readFATEntry(i, &entry);
    if (strcmp(entry.filename, name) == 0) {
      return i;
    }
  }
  return -1;
  }
  void storeFile() {
  FATEntry newFile;
  if (fileExists(parameter1)) {
    Serial.print("Filename is already taken");
    return;
  }
  int fileSize = atoi(parameter2);
  //Check of er al bestanden in systeem staan, zo niet is het addres 161
  if (noOfFiles == 0) {
    int address = firstUseableAdd;
    strcpy(newFile.filename, parameter1);
    newFile.startfileaddr = address; //achrijf een functie die een adres toewijst
    newFile.length = fileSize;

    writeFATEntry(newFile);
    EEPROM.put(address, data);

    Serial.print("Filename:\t");
    Serial.println(newFile.filename);
    Serial.print("Size:\t\t");
    Serial.println(newFile.length);
    Serial.println("File stored\n");
    return;
  }
  //Kijk of er plek is boven het laatste blok
  //  else if ((EEPROM.length() - (FAT[noOfFiles - 1].startfileaddr + FAT[noOfFiles - 1].length)) > fileSize) {
  else {
    int address;
    for(int i = 0; i < noOfFiles ; i++) {
      FATEntry entry;
      readFATEntry(i, &entry);
      address+= entry.length;
      writeFATEntry(entry);
    }

  //    int address = (FAT[noOfFiles - 1].startfileaddr + FAT[noOfFiles - 1].length) + 1;
    Serial.println(FAT[noOfFiles - 1].startfileaddr);
    Serial.println(FAT[noOfFiles - 1].length);
    Serial.println(address + 1);
    strcpy(newFile.filename, parameter1);
    newFile.startfileaddr = address + 1;
    newFile.length = fileSize;

    writeFATEntry(newFile);
    EEPROM.put(address, data);

    Serial.print("Filename:\t");
    Serial.println(newFile.filename);
    Serial.print("Size:\t\t");
    Serial.println(newFile.length);
    Serial.println("File stored\n");
    return;
  }
  //  else {
  //    Serial.print("Not enough space left for file");
  //  }
  }
  void retrieveFile() {
  int address = -1;
  int fileIndex;
  FATEntry entry;
  if (fileExists(parameter1)) {
    fileIndex = entryIndex(parameter1);
    readFATEntry(fileIndex, &entry);
    address = entry.startPosition;
  }
  if (address == -1) {
    Serial.println(F("File does not exist"));
    return;
  }
  else {
    Serial.print(F("File retrieved: \t"));
    Serial.println(entry.filename);
    char character[entry.length];
    character[0] = '\0';
    for (int i = 0; i < entry.length; i++) {
      character[i] = EEPROM.read(address++);
      Serial.print(character[i]);
    }
  }
  }
  void eraseFile() {
  if (!fileExists(parameter1)) {
    Serial.println(F("File does not exist"));
    return;
  }
  FATEntry entry;
  int index = entryIndex(parameter1);
  Serial.println(index);
  //  alles entry het laatste bestand is dan ...
  if (index == noOfFiles - 1) {
    //    int tempNoOfFiles = noOfFiles;
    //    //      noOfFiles = 0;
    //    for (int i = 0; i < tempNoOfFiles - 1; i ++) {
    //      FATEntry entry;
    //      readFATEntry(i, &entry);
    //      writeFATEntry(entry);
    //    }

    noOfFiles--;
  }
  else {
    for (int i = index; i < noOfFiles ; i ++) {
      FATEntry entry;
      readFATEntry(index + 1, &entry);
      noOfFiles--;
      writeFATEntry(entry);
    }
  }
  Serial.print(F("File erased: \t"));
  Serial.println(parameter1);
  Serial.println();
  }
  void showFiles() {
  if (noOfFiles == 0) {
    Serial.println(F("No files in memory"));
    return;
  }
  FATEntry entry;
  Serial.print(F("Showing all files \n"));
  Serial.print(F("Number of files: \t"));
  Serial.println(noOfFiles);
  for (int i = 0; i < noOfFiles; i++) {
    readFATEntry(i, &entry);
    Serial.print(F("Filename: \t"));
    Serial.println(entry.filename);
    Serial.print(F("File size: \t"));
    Serial.println(entry.length);
    Serial.print(F("File address: \t"));
    Serial.println(entry.startPosition);
    Serial.println();
  }
  }

  // */
