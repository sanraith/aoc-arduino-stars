#pragma once

#include <NTPClient.h>
#include "WiFiS3.h"

class EEPROMManager
{
public:
    EEPROMManager(int version, int startAddress);

    /**
     * Should be called in the same order every time to ensure the same memory layout.
     * @return the address of the slot.
     */
    int registerSlot(int size);

    /**
     * Should be called after all registerSlot calls are done.
     * Checks if the memory has the same layout based on the version and clears the used memory if not.
     */
    void setup();

private:
    int _version;
    int _allocatedSize;
    int _startAddress;
};