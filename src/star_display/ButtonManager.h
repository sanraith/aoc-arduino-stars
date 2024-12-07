
#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

typedef void (*ButtonCallback)();

class ButtonManager
{
public:
    ButtonManager(int pin);
    void setup(ButtonCallback onPress, ButtonCallback onRelease);
    void loop();

private:
    int pin;
    ButtonCallback onPress;
    ButtonCallback onRelease;
    bool lastButtonState;
};

#endif // BUTTON_MANAGER_H