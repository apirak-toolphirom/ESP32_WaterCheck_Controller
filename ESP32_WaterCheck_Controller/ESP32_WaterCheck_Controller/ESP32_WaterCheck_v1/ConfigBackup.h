#ifndef CONFIG_BACKUP_H
#define CONFIG_BACKUP_H

#include <Arduino.h>

String configExportJson();
bool configImportJson(const String& json);

#endif
