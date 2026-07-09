#include "EventLog.h"
#include "config.h"
#include "TimeSync.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

static bool fsReady = false;

static String csvEscape(const String& s) {
  String out = s;
  out.replace("\"", "\"\"");
  return "\"" + out + "\"";
}

static void rotateIfNeeded() {
  if (!fsReady) return;
  if (!LittleFS.exists(EVENT_LOG_FILE)) return;

  File f = LittleFS.open(EVENT_LOG_FILE, "r");
  if (!f) return;
  size_t sz = f.size();
  f.close();

  if (sz <= EVENT_LOG_MAX_BYTES) return;

  if (LittleFS.exists(EVENT_LOG_BACKUP_FILE)) {
    LittleFS.remove(EVENT_LOG_BACKUP_FILE);
  }
  LittleFS.rename(EVENT_LOG_FILE, EVENT_LOG_BACKUP_FILE);

  File nf = LittleFS.open(EVENT_LOG_FILE, "w");
  if (nf) {
    nf.println("timestamp,type,message");
    nf.close();
  }
}

void eventLogBegin() {
  fsReady = LittleFS.begin(true);
  if (!fsReady) return;

  if (!LittleFS.exists(EVENT_LOG_FILE)) {
    File f = LittleFS.open(EVENT_LOG_FILE, "w");
    if (f) {
      f.println("timestamp,type,message");
      f.close();
    }
  }
}

void eventLogAdd(const String& type, const String& message) {
  if (!fsReady) return;
  rotateIfNeeded();

  File f = LittleFS.open(EVENT_LOG_FILE, "a");
  if (!f) return;

  String ts = dateTimeNowString();
  f.print(csvEscape(ts));
  f.print(",");
  f.print(csvEscape(type));
  f.print(",");
  f.println(csvEscape(message));
  f.close();
}

String eventLogReadCsv() {
  if (!fsReady || !LittleFS.exists(EVENT_LOG_FILE)) return "timestamp,type,message\n";

  File f = LittleFS.open(EVENT_LOG_FILE, "r");
  if (!f) return "timestamp,type,message\n";

  String out;
  while (f.available()) {
    out += (char)f.read();
    if (out.length() > EVENT_LOG_MAX_BYTES + 2048) break;
  }
  f.close();
  return out;
}

String eventLogReadJson(uint16_t limit) {
  if (!fsReady || !LittleFS.exists(EVENT_LOG_FILE)) return "[]";

  File f = LittleFS.open(EVENT_LOG_FILE, "r");
  if (!f) return "[]";

  DynamicJsonDocument doc(16384);
  JsonArray arr = doc.to<JsonArray>();

  bool header = true;
  uint16_t count = 0;

  while (f.available() && count < limit) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (!line.length()) continue;
    if (header) {
      header = false;
      continue;
    }

    // Simple CSV parser สำหรับรูปแบบ "timestamp","type","message"
    int p1 = line.indexOf("\",\"");
    int p2 = line.indexOf("\",\"", p1 + 3);
    if (p1 < 0 || p2 < 0) continue;

    String ts = line.substring(1, p1);
    String type = line.substring(p1 + 3, p2);
    String msg = line.substring(p2 + 3);
    if (msg.startsWith("\"")) msg.remove(0, 1);
    if (msg.endsWith("\"")) msg.remove(msg.length() - 1);

    JsonObject o = arr.createNestedObject();
    o["timestamp"] = ts;
    o["type"] = type;
    o["message"] = msg;
    count++;
  }

  f.close();

  String out;
  serializeJson(arr, out);
  return out;
}

void eventLogClear() {
  if (!fsReady) return;
  LittleFS.remove(EVENT_LOG_FILE);
  File f = LittleFS.open(EVENT_LOG_FILE, "w");
  if (f) {
    f.println("timestamp,type,message");
    f.close();
  }
}

size_t eventLogSize() {
  if (!fsReady || !LittleFS.exists(EVENT_LOG_FILE)) return 0;
  File f = LittleFS.open(EVENT_LOG_FILE, "r");
  if (!f) return 0;
  size_t sz = f.size();
  f.close();
  return sz;
}
