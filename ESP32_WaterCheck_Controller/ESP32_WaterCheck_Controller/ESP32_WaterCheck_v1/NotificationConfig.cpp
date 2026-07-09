#include "NotificationConfig.h"
#include <Preferences.h>

static Preferences prefs;

bool notifyTelegramEnabled = false;
String notifyTelegramToken = "";
String notifyTelegramChatId = "";

bool notifyLineEnabled = false;
String notifyLineToken = "";

bool notifyEmailEnabled = false;
String notifySmtpHost = "";
uint16_t notifySmtpPort = 587;
String notifySmtpUser = "";
String notifySmtpPass = "";
String notifyEmailTo = "";

void notificationConfigLoad() {
  prefs.begin("notify", false);

  notifyTelegramEnabled = prefs.getBool("tgEnable", false);
  notifyTelegramToken = prefs.getString("tgToken", "");
  notifyTelegramChatId = prefs.getString("tgChat", "");

  notifyLineEnabled = prefs.getBool("lnEnable", false);
  notifyLineToken = prefs.getString("lnToken", "");

  notifyEmailEnabled = prefs.getBool("emEnable", false);
  notifySmtpHost = prefs.getString("smtpHost", "");
  notifySmtpPort = prefs.getUShort("smtpPort", 587);
  notifySmtpUser = prefs.getString("smtpUser", "");
  notifySmtpPass = prefs.getString("smtpPass", "");
  notifyEmailTo = prefs.getString("emailTo", "");
}

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
) {
  notifyTelegramEnabled = telegramEnable;
  notifyTelegramToken = telegramToken;
  notifyTelegramChatId = telegramChatId;

  notifyLineEnabled = lineEnable;
  notifyLineToken = lineToken;

  notifyEmailEnabled = emailEnable;
  notifySmtpHost = smtpHost;
  notifySmtpPort = smtpPort;
  notifySmtpUser = smtpUser;
  notifySmtpPass = smtpPass;
  notifyEmailTo = emailTo;

  prefs.putBool("tgEnable", notifyTelegramEnabled);
  prefs.putString("tgToken", notifyTelegramToken);
  prefs.putString("tgChat", notifyTelegramChatId);

  prefs.putBool("lnEnable", notifyLineEnabled);
  prefs.putString("lnToken", notifyLineToken);

  prefs.putBool("emEnable", notifyEmailEnabled);
  prefs.putString("smtpHost", notifySmtpHost);
  prefs.putUShort("smtpPort", notifySmtpPort);
  prefs.putString("smtpUser", notifySmtpUser);
  prefs.putString("smtpPass", notifySmtpPass);
  prefs.putString("emailTo", notifyEmailTo);
}
