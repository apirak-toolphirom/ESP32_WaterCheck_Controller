#ifndef FLOW_SWITCH_H
#define FLOW_SWITCH_H

#include <Arduino.h>

void flowBegin();
bool flowReadRaw();
bool flowUpdate();
bool flowIsOn();

#endif
