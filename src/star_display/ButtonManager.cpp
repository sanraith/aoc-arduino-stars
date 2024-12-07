
#include "ButtonManager.h"

ButtonManager::ButtonManager(int pin) : pin(pin), onPress(nullptr), onRelease(nullptr), lastButtonState(LOW) {}

void ButtonManager::setup(ButtonCallback onPress, ButtonCallback onRelease)
{
    this->onPress = onPress;
    this->onRelease = onRelease;
    pinMode(pin, INPUT);
}

void ButtonManager::loop()
{
    bool buttonState = digitalRead(pin);
    if (buttonState != lastButtonState)
    {
        if (buttonState == HIGH && onPress)
        {
            onPress();
        }
        else if (buttonState == LOW && onRelease)
        {
            onRelease();
        }
        lastButtonState = buttonState;
    }
}