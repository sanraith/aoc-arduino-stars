#include "EEPROMManager.h"
#include <EEPROM.h>

EEPROMManager::EEPROMManager(int version, int startAddress)
{
    _version = version;
    _startAddress = startAddress;
    _allocatedSize = sizeof(_startAddress); // save version number in the first slot
    if (_startAddress + _allocatedSize > EEPROM.length())
    {
        // throw "Trying to over-allocate EEPROM!";
    }
    EEPROM.get(_startAddress, _storedVersion);
}

void EEPROMManager::setup()
{
    if (_version != _storedVersion)
    {
        EEPROM.put(_startAddress, _version);
        Serial.println("EEPROM version mismatch, using defaults.");
    }

    Serial.print("EEPROM using address space ");
    Serial.print(_startAddress);
    Serial.print("..");
    Serial.print(_startAddress + _allocatedSize);
    Serial.print(" out of ");
    Serial.print(EEPROM.length());
    Serial.println(" bytes.");
}
