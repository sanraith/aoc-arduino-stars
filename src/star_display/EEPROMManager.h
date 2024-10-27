#pragma once

#include <NTPClient.h>
#include <EEPROM.h>
#include "WiFiS3.h"

class EEPROMManager
{
public:
    EEPROMManager(int version, int startAddress);

    /**
     * Should be called in the same order every time to ensure the same memory layout.
     * @return the address of the slot.
     */
    template <typename T>
    int registerSlot(int size, const T &defaultValue);

    /**
     * Should be called after all registerSlot calls are done.
     * Checks if the memory has the same layout based on the version and clears the used memory if not.
     */
    void setup();

private:
    int _version;
    int _allocatedSize;
    int _startAddress;
    int _storedVersion;
};

template <typename T>
int EEPROMManager::registerSlot(int size, const T &currentValue)
{
    int address = _startAddress + _allocatedSize;
    _allocatedSize += size;
    if (address + size > EEPROM.length())
    {
        // throw "Trying to over-allocate EEPROM!";
    }

    if (_version == _storedVersion)
    {
        EEPROM.get(address, currentValue);
    }
    else
    {
        EEPROM.put(address, currentValue);
    }

    char msg[80];
    snprintf(msg, sizeof(msg), "Registered EEPROM slot at %d with size %d", address, size);
    Serial.println(msg);

    return address;
}

// Explicit template instantiation for common types
template int EEPROMManager::registerSlot<int>(int size, const int &currentValue);
template int EEPROMManager::registerSlot<float>(int size, const float &currentValue);
template int EEPROMManager::registerSlot<double>(int size, const double &currentValue);
template int EEPROMManager::registerSlot<char>(int size, const char &currentValue);
template int EEPROMManager::registerSlot<String>(int size, const String &currentValue);