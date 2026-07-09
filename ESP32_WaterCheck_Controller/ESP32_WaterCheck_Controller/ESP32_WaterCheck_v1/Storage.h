#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

void storageBegin();
void storageLoad();
void storageSaveDelay(uint32_t seconds);
void storageSavePumpTest(uint32_t seconds);
void storageSaveMqtt(const String& server, uint16_t port, const String& user, const String& pass, const String& topic);
void storageSaveAdmin(const String& user, const String& pass);

#endif
