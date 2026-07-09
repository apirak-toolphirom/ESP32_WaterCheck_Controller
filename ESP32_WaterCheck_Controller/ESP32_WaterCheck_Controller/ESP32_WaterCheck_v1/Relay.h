#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

void relayBegin();
void setPump(bool on);
void setAuxRelay(bool on);
bool getPumpRelay();
bool getAuxRelay();

#endif
