#ifndef NOTIFICATION_CONFIG_H
#define NOTIFICATION_CONFIG_H

#include <Arduino.h>

void notificationConfigLoad();
void notificationConfigSave(
  bool telegramEnable,
  const String& telegramToken,
  const String& telegramChatId,
  bool lineEnable,
  const String& lineToken,
  bool emailEnable,
  const String& smtpHost,
  uint16_t smtpPort,
  const String& smtpUser,
  const String& smtpPass,
  const String& emailTo
);

extern bool notifyTelegramEnabled;
extern String notifyTelegramToken;
extern String notifyTelegramChatId;

extern bool notifyLineEnabled;
extern String notifyLineToken;

extern bool notifyEmailEnabled;
extern String notifySmtpHost;
extern uint16_t notifySmtpPort;
extern String notifySmtpUser;
extern String notifySmtpPass;
extern String notifyEmailTo;

#endif
