#ifndef STORAGE_H
#define STORAGE_H
#include <Arduino.h>
#include "global.h"

void storageBegin();
void loadSettings();
void saveDelay(uint32_t delaySec);
void savePumpTest(uint32_t pumpTestSec);
void saveMqtt(const String& server, uint16_t port, const String& user, const String& pass, const String& topic);
void saveAdmin(const String& user, const String& pass);
void factoryResetSettings();

#endif
