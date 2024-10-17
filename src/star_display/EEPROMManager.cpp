#include "EEPROMManager.h"
#include <EEPROM.h>

EEPROMManager::EEPROMManager(int version, int startAddress)
{
    _version = version;
    _startAddress = startAddress;
    _allocatedSize = sizeof(int); // save version number in the first slot
    if (_startAddress + _allocatedSize > EEPROM.length())
    {
        // throw "Trying to over-allocate EEPROM!";
    }
}

int EEPROMManager::registerSlot(int size)
{
    int address = _startAddress + _allocatedSize;
    _allocatedSize += size;
    if (address + size > EEPROM.length())
    {
        // throw "Trying to over-allocate EEPROM!";
    }

    return address;
}

void EEPROMManager::setup()
{
    int storedVersion;
    EEPROM.get(_startAddress, storedVersion);
    if (storedVersion != _version)
    {
        Serial.println("Stored memory version does not match current version, clearing bytes: " + String(_allocatedSize));
        EEPROM.put(_startAddress, _version);
        for (int addr = _startAddress + sizeof(int); addr < _startAddress + _allocatedSize; addr++)
        {
            EEPROM.update(addr, 0);
        }
    }

    Serial.print("EEPROM using address space ");
    Serial.print(_startAddress);
    Serial.print("..");
    Serial.print(_startAddress + _allocatedSize);
    Serial.print(" out of ");
    Serial.print(EEPROM.length());
    Serial.println(" bytes.");
}
