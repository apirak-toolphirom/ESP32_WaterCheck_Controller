#include "Dashboard.h"
#include "global.h"
#include "config.h"
#include "Storage.h"
#include "HealthMonitor.h"
#include "ConfigBackup.h"
#include "OTAUpdate.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <base64.h>

static WebServer server(80);

static bool isAuthenticated() {
  if (!server.hasHeader("Authorization")) return false;
  String auth = server.header("Authorization");
  String expected = "Basic " + base64::encode(adminUser + ":" + adminPass);
  return auth == expected;
}

static bool requireAuth() {
  if (isAuthenticated()) return true;
  server.sendHeader("WWW-Authenticate", "Basic realm=\"WaterCheck Config\"");
  server.send(401, "text/plain", "Authentication required");
  return false;
}

static void sendStatus() {
  StaticJsonDocument<1024> doc;
  doc["title"] = DASHBOARD_TITLE;
  doc["version"] = FIRMWARE_VERSION;
  doc["flow"] = flowState;
  doc["pump"] = pumpRelayState;
  doc["state"] = (int)pumpState;
  doc["alarm"] = alarmActive;
  doc["estop"] = estopActive;
  doc["maintenance"] = maintenanceMode;
  doc["relayFail"] = relayFailAlarm;
  doc["manualMode"] = (int)manualMode;
  doc["retry"] = currentRetryCount;
  doc["maxRetry"] = maxRetryCount;
  doc["delay"] = delayTimeSec;
  doc["test"] = pumpTestTimeSec;
  doc["relayFailSec"] = relayFailTimeoutSec;
  doc["remain"] = remainTimeSec;
  doc["mqtt"] = mqttConnected;
  doc["rssi"] = WiFi.RSSI();
  doc["ip"] = WiFi.localIP().toString();
  doc["lastEvent"] = lastEvent;
  doc["lastAlarm"] = lastAlarmTime;
  doc["restart"] = lastRestartReason;

  JsonObject health = doc.createNestedObject("health");
  health["uptimeSec"] = healthUptimeSec();
  health["freeHeap"] = healthFreeHeap();
  health["minFreeHeap"] = healthMinFreeHeap();
  health["chipTempC"] = healthChipTempC();
  health["freeSketchSpace"] = ESP.getFreeSketchSpace();

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

static void handleSave() {
  if (!requireAuth()) return;
  if (server.hasArg("delay")) storageSaveDelay(server.arg("delay").toInt());
  if (server.hasArg("test")) storageSavePumpTest(server.arg("test").toInt());
  if (server.hasArg("retry")) storageSaveMaxRetry((uint8_t)server.arg("retry").toInt());
  if (server.hasArg("relayFailSec")) storageSaveRelayFailTimeout(server.arg("relayFailSec").toInt());

  if (server.hasArg("mqttServer")) {
    storageSaveMqtt(
      server.arg("mqttServer"),
      server.hasArg("mqttPort") ? server.arg("mqttPort").toInt() : MQTT_DEFAULT_PORT,
      server.arg("mqttUser"),
      server.arg("mqttPass"),
      server.arg("mqttTopic")
    );
  }
  server.send(200, "text/plain", "OK");
}

static void handleUnlock() {
  if (!requireAuth()) return;
  alarmActive = false;
  relayFailAlarm = false;
  currentRetryCount = 0;
  lastEvent = "Unlock from dashboard";
  pumpState = PUMP_DELAY_WAIT;
  stateStartMs = millis();
  remainTimeSec = delayTimeSec;
  server.send(200, "text/plain", "UNLOCKED");
}

static void handleMode() {
  if (!requireAuth()) return;
  String mode = server.arg("mode");
  if (mode == "auto") manualMode = MODE_AUTO;
  else if (mode == "on") manualMode = MODE_MANUAL_ON;
  else if (mode == "off") manualMode = MODE_MANUAL_OFF;
  else if (mode == "maint_on") maintenanceMode = true;
  else if (mode == "maint_off") maintenanceMode = false;
  server.send(200, "text/plain", "OK");
}

static void handleExportConfig() {
  if (!requireAuth()) return;
  server.sendHeader("Content-Disposition", "attachment; filename=watercheck_config.json");
  server.send(200, "application/json", configExportJson());
}

static void handleImportConfig() {
  if (!requireAuth()) return;
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Missing JSON body");
    return;
  }
  bool ok = configImportJson(server.arg("plain"));
  server.send(ok ? 200 : 400, "text/plain", ok ? "IMPORTED" : "INVALID JSON");
}

static void handleHealth() {
  server.send(200, "application/json", healthJson());
}

static void handleOtaInfo() {
  if (!requireAuth()) return;
  server.send(200, "application/json", otaPartitionInfoJson());
}

static void handleFirmwareUploadDone() {
  if (!requireAuth()) return;
  bool ok = !Update.hasError();
  server.send(ok ? 200 : 500, "text/plain", ok ? "Update OK. Rebooting..." : "Update Failed");
  if (ok) {
    delay(1000);
    ESP.restart();
  }
}

static void handleFirmwareUpload() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("OTA upload start: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      Serial.printf("OTA upload success: %u bytes\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
  }
}

static void handleRoot() {
  String html = R"HTML(
<!doctype html><html lang="th"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>ระบบเช็คน้ำขาด by ช่างหนึ่ง</title>
<style>
body{font-family:Arial,sans-serif;background:#f4f7fb;margin:20px;color:#222}
.card{background:#fff;padding:16px;border-radius:12px;margin-bottom:12px;box-shadow:0 2px 8px #0001}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:10px}
.value{font-size:24px;font-weight:bold}.ok{background:#e9ffe9}.alarm{background:#ffe5e5;border:1px solid #ff8888}
.warn{background:#fff5d6;border:1px solid #e0b030}input,button,textarea{padding:9px;margin:4px;width:100%;box-sizing:border-box}
button{border:0;border-radius:8px;background:#0b65c2;color:#fff;cursor:pointer}.danger{background:#c62828}.gray{background:#555}
textarea{height:140px}
</style></head><body>
<h1>ระบบเช็คน้ำขาด by ช่างหนึ่ง</h1>
<div id="banner" class="card"></div>
<div class="grid">
<div class="card"><small>Flow</small><div id="flow" class="value">-</div></div>
<div class="card"><small>Pump</small><div id="pump" class="value">-</div></div>
<div class="card"><small>State</small><div id="state" class="value">-</div></div>
<div class="card"><small>Countdown</small><div id="remain" class="value">-</div></div>
<div class="card"><small>Retry</small><div id="retry" class="value">-</div></div>
<div class="card"><small>WiFi</small><div id="wifi" class="value">-</div></div>
</div>
<div class="card">
<h2>Health Monitor</h2>
<pre id="health"></pre>
</div>
<div class="card">
<h2>Config</h2>
<label>Delay Time (sec)<input id="delay" type="number"></label>
<label>Pump Test Time (sec)<input id="test" type="number"></label>
<label>Max Retry<input id="retrySet" type="number"></label>
<label>Relay Fail Timeout (sec)<input id="relayFailSec" type="number"></label>
<label>MQTT Server<input id="mqttServer"></label>
<label>MQTT Port<input id="mqttPort" type="number" value="1883"></label>
<label>MQTT User<input id="mqttUser"></label>
<label>MQTT Password<input id="mqttPass" type="password"></label>
<label>MQTT Topic<input id="mqttTopic" value="water/check"></label>
<button onclick="save()">Save Config</button>
</div>
<div class="card">
<h2>Backup / Restore Config</h2>
<button onclick="location.href='/api/config/export'">Download Config JSON</button>
<textarea id="importBox" placeholder="วาง JSON config ที่ต้องการ Restore"></textarea>
<button onclick="importConfig()">Import Config JSON</button>
</div>
<div class="card">
<h2>OTA Firmware Update</h2>
<form method="POST" action="/api/ota/upload" enctype="multipart/form-data">
<input type="file" name="firmware" accept=".bin">
<button type="submit">Upload Firmware .bin</button>
</form>
<button onclick="otaInfo()">Show OTA Partition Info</button>
<pre id="ota"></pre>
</div>
<div class="card">
<h2>Control</h2>
<button onclick="mode('auto')">AUTO</button>
<button onclick="mode('on')">Manual Pump ON</button>
<button onclick="mode('off')">Manual Pump OFF</button>
<button onclick="mode('maint_on')" class="gray">Maintenance ON</button>
<button onclick="mode('maint_off')" class="gray">Maintenance OFF</button>
<button onclick="unlock()" class="danger">Unlock / Reset Alarm</button>
<button onclick="restart()" class="danger">Restart ESP32</button>
</div>
<div class="card"><h2>Event</h2><pre id="event"></pre></div>
<script>
function stateName(n){return ['RUN','WAIT','TEST','LOCKOUT','ESTOP','MAINT'][n]||'UNK'}
async function load(){
 const r=await fetch('/api/status'); const j=await r.json();
 flow.textContent=j.flow?'ON':'OFF'; pump.textContent=j.pump?'ON':'OFF'; state.textContent=stateName(j.state);
 remain.textContent=j.remain+'s'; retry.textContent=j.retry+'/'+j.maxRetry; wifi.textContent=j.rssi+' dBm';
 delay.value=j.delay; test.value=j.test; retrySet.value=j.maxRetry; relayFailSec.value=j.relayFailSec;
 let cls='card ok', msg='<h2>System OK</h2>';
 if(j.estop){cls='card alarm';msg='<h2>E-STOP ACTIVE</h2>'}
 else if(j.relayFail){cls='card alarm';msg='<h2>RELAY FAIL ALARM</h2>'}
 else if(j.alarm){cls='card alarm';msg='<h2>LOCKOUT ALARM</h2>'}
 else if(j.maintenance){cls='card warn';msg='<h2>MAINTENANCE MODE</h2>'}
 banner.className=cls; banner.innerHTML=msg;
 health.textContent=JSON.stringify(j.health,null,2);
 event.textContent='Version: '+j.version+'\\nIP: '+j.ip+'\\nMQTT: '+j.mqtt+'\\nManualMode: '+j.manualMode+'\\nLastEvent: '+j.lastEvent+'\\nLastAlarm: '+j.lastAlarm+'\\nRestart: '+j.restart;
}
async function save(){const fd=new FormData();['delay','test','retrySet','relayFailSec','mqttServer','mqttPort','mqttUser','mqttPass','mqttTopic'].forEach(id=>fd.append(id=='retrySet'?'retry':id,document.getElementById(id).value));await fetch('/api/save',{method:'POST',body:fd});alert('Saved')}
async function unlock(){await fetch('/api/unlock',{method:'POST'});load()}
async function mode(m){const fd=new FormData();fd.append('mode',m);await fetch('/api/mode',{method:'POST',body:fd});load()}
async function restart(){if(confirm('Restart ESP32?')) await fetch('/api/restart',{method:'POST'});}
async function importConfig(){let txt=importBox.value;let r=await fetch('/api/config/import',{method:'POST',headers:{'Content-Type':'application/json'},body:txt});alert(await r.text());}
async function otaInfo(){let r=await fetch('/api/ota/info');ota.textContent=await r.text();}
setInterval(load,1000);load();
</script></body></html>
)HTML";
  server.send(200, "text/html; charset=utf-8", html);
}

static void handleRestart() {
  if (!requireAuth()) return;
  server.send(200, "text/plain", "Restarting");
  delay(500);
  ESP.restart();
}

void dashboardBegin() {
  const char* headerKeys[] = {"Authorization"};
  server.collectHeaders(headerKeys, 1);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, sendStatus);
  server.on("/api/save", HTTP_POST, handleSave);
  server.on("/api/unlock", HTTP_POST, handleUnlock);
  server.on("/api/mode", HTTP_POST, handleMode);
  server.on("/api/restart", HTTP_POST, handleRestart);

  server.on("/api/health", HTTP_GET, handleHealth);
  server.on("/api/config/export", HTTP_GET, handleExportConfig);
  server.on("/api/config/import", HTTP_POST, handleImportConfig);
  server.on("/api/ota/info", HTTP_GET, handleOtaInfo);
  server.on("/api/ota/upload", HTTP_POST, handleFirmwareUploadDone, handleFirmwareUpload);

  server.begin();
}

void dashboardLoop() {
  server.handleClient();
}
