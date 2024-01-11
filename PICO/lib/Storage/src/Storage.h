#pragma once

#include <Arduino.h>

#define EEPROM_SIZE 2000
#define STORED_PROGRAM_OFFSET 20

#define PROGRAM_STATUS_BYTE_OFFSET 0
#define PROGRAM_STORED_VALUE1 0xaa
#define PROGRAM_STORED_VALUE2 0x55

#define WHEEL_SETTINGS_OFFSET 2

#define DEBUG_STORE_BLOCK_IN_EEPROM

void setupStorage();

// Stores a program byte into the eeprom at the stated location
// The pos value is the offset in the EEProm into which the program is to be written
// The function returns true if the byte was stored, false if not

bool storeByteIntoEEPROM(uint8_t byte, int pos);

uint8_t readByteFromEEPROM(int pos);

bool storeBlockIntoEEPROM(uint8_t * blockStart, int length, int pos);

bool loadBlockFromEEPROM(uint8_t * blockStart, int length, int pos);

// Stores a program into the eeprom at the stated location
// The program is a string of text which is zero terminated
// The EEPromStart value is the offset in the EEProm into which the program is to be written
// The function returns true if the program was loaded, false if not

bool storeProgramIntoEEPROM(char * programStart, int EEPromStart);

void setProgramStored();

void clearProgramStoredFlag();

bool isProgramStored();

