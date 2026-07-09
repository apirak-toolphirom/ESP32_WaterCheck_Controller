#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include <Arduino.h>

void timeSyncBegin();
bool timeIsReady();
String timeNowString();
String dateTimeNowString();

#endif
