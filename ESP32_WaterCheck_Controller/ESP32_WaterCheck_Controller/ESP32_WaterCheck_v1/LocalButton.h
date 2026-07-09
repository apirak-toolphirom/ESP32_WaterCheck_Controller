#ifndef LOCAL_BUTTON_H
#define LOCAL_BUTTON_H

#include <Arduino.h>

void localButtonBegin();
bool localUnlockPressed();
bool factoryResetPressed();
bool estopIsPressed();

#endif
