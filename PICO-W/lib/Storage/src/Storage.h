#pragma once

#include <Arduino.h>

#define EEPROM_SIZE 2000
#define PROGRAM_STATUS_BYTE_OFFSET 0
#define PROGRAM_STORED_VALUE1 0xaa
#define PROGRAM_STORED_VALUE2 0x55

#define WHEEL_SETTINGS_STORED 0x55

struct WheelSettings
{
  int leftWheelDiameter;
  int rightWheelDiameter;
  int wheelSpacing;
  char check;
};

#define NETWORK_SETTING_LENGTH 20

#define WIFI_NETWORK_SSID_OFFSET 2

#define WIFI_NETWORK_PASSWORD_OFFSET (WIFI_NETWORK_SSID_OFFSET+NETWORK_SETTING_LENGTH+1)

#define WIFI_NETWORK_NETWORK_NAME_OFFSET (WIFI_NETWORK_PASSWORD_OFFSET+NETWORK_SETTING_LENGTH+1)

#define WHEEL_SETTINGS_OFFSET (WIFI_NETWORK_NETWORK_NAME_OFFSET+NETWORK_SETTING_LENGTH+1)

#define STORED_PROGRAM_OFFSET WHEEL_SETTINGS_OFFSET + sizeof(struct WheelSettings)

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

bool commitWriteToEEPROM();

void setProgramStored();

void clearProgramStoredFlag();

bool isProgramStored();

