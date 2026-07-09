#include "Notification.h"
#include "NotificationConfig.h"
#include "MQTTClient.h"
#include "EventLog.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

static String lastNotifyStatus = "idle";
static uint32_t lastNotifyMs = 0;

// หมายเหตุ: การส่ง Email ด้วย SMTP ต้องใช้ Library เพิ่ม เช่น ESP_Mail_Client
// ใน Sprint 1.8 นี้เตรียมโครงสร้าง Config + Status ให้ก่อน และทำ Telegram/LINE ผ่าน HTTPS

static String urlEncode(const String& value) {
  String encoded = "";
  char c;
  char code0;
  char code1;
  for (unsigned int i = 0; i < value.length(); i++) {
    c = value.charAt(i);
    if (isalnum(c)) {
      encoded += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) code1 = (c & 0xf) - 10 + 'A';
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) code0 = c - 10 + 'A';
      encoded += '%';
      encoded += code0;
      encoded += code1;
    }
  }
  return encoded;
}

static bool sendTelegram(const String& message) {
  if (!notifyTelegramEnabled || !notifyTelegramToken.length() || !notifyTelegramChatId.length()) return false;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  String url = "https://api.telegram.org/bot" + notifyTelegramToken + "/sendMessage";
  if (!https.begin(client, url)) return false;

  https.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String body = "chat_id=" + urlEncode(notifyTelegramChatId) + "&text=" + urlEncode(message);
  int code = https.POST(body);
  https.end();

  return code >= 200 && code < 300;
}

static bool sendLine(const String& message) {
  if (!notifyLineEnabled || !notifyLineToken.length()) return false;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  // หมายเหตุ: LINE Notify ถูกแทนที่ด้วย LINE Messaging API ในงานจริงควรใช้ Messaging API
  // endpoint นี้ใช้สำหรับ token compatible/legacy หรือ proxy ที่ผู้ใช้ตั้งเอง
  String url = "https://notify-api.line.me/api/notify";
  if (!https.begin(client, url)) return false;

  https.addHeader("Authorization", "Bearer " + notifyLineToken);
  https.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int code = https.POST("message=" + urlEncode(message));
  https.end();

  return code >= 200 && code < 300;
}

void notifyBegin() {
  notificationConfigLoad();
  eventLogAdd("SYSTEM", "Notification manager started");
}

void notifyLoop() {
  // Reserved for retry queue in future sprint
}

void notifyAlarm(const String& type, const String& message) {
  String full = "[WaterCheck] " + type + ": " + message;
  eventLogAdd(type, message);
  mqttPublishAlarm(full);

  bool tg = sendTelegram(full);
  bool ln = sendLine(full);

  if (notifyEmailEnabled) {
    // SMTP framework prepared, implementation deferred
    lastNotifyStatus = "telegram=" + String(tg) + ", line=" + String(ln) + ", email=deferred";
  } else {
    lastNotifyStatus = "telegram=" + String(tg) + ", line=" + String(ln);
  }
  lastNotifyMs = millis();
}

void notifyEvent(const String& type, const String& message) {
  eventLogAdd(type, message);
}

String notifyStatusJson() {
  StaticJsonDocument<384> doc;
  doc["telegramEnabled"] = notifyTelegramEnabled;
  doc["lineEnabled"] = notifyLineEnabled;
  doc["emailEnabled"] = notifyEmailEnabled;
  doc["lastStatus"] = lastNotifyStatus;
  doc["lastNotifyMs"] = lastNotifyMs;
  String out;
  serializeJson(doc, out);
  return out;
}
