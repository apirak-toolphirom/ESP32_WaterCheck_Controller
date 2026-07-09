#include "Dashboard.h"
#include "global.h"
#include "config.h"
#include "Storage.h"
#include "HealthMonitor.h"
#include "ConfigBackup.h"
#include "OTAUpdate.h"
#include "EventLog.h"
#include "SystemMonitor.h"
#include "GraphHistory.h"
#include "Notification.h"
#include "NotificationConfig.h"

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
  StaticJsonDocument<1536> doc;
  doc["title"] = DASHBOARD_TITLE;
  doc["version"] = FIRMWARE_VERSION;
  doc["build"] = BUILD_NAME;
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
  doc["lastFlowStop"] = lastFlowStopTime;
  doc["lastPumpTest"] = lastPumpTestTime;
  doc["lastFlowReturn"] = lastFlowReturnTime;
  doc["lastAlarm"] = lastAlarmTime;
  doc["restart"] = lastRestartReason;
  doc["eventLogSize"] = eventLogSize();

  JsonObject health = doc.createNestedObject("health");
  health["uptimeSec"] = healthUptimeSec();
  health["freeHeap"] = healthFreeHeap();
  health["minFreeHeap"] = healthMinFreeHeap();
  health["chipTempC"] = healthChipTempC();
  health["freeSketchSpace"] = ESP.getFreeSketchSpace();

  JsonObject sys = doc.createNestedObject("system");
  sys["loopCounter"] = systemLoopCounter();
  sys["loopAvgUs"] = systemLoopAvgUs();
  sys["wifiDisconnects"] = systemWifiDisconnectCount();
  sys["mqttReconnects"] = systemMqttReconnectCount();
  sys["restarts"] = systemRestartCount();

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

  eventLogAdd("CONFIG", "Configuration saved from dashboard");
  server.send(200, "text/plain", "OK");
}

static void handleUnlock() {
  if (!requireAuth()) return;
  alarmActive = false;
  relayFailAlarm = false;
  currentRetryCount = 0;
  lastEvent = "Unlock from dashboard";
  eventLogAdd("UNLOCK", "Unlock from dashboard");
  pumpState = PUMP_DELAY_WAIT;
  stateStartMs = millis();
  remainTimeSec = delayTimeSec;
  server.send(200, "text/plain", "UNLOCKED");
}

static void handleMode() {
  if (!requireAuth()) return;
  String mode = server.arg("mode");
  if (mode == "auto") { manualMode = MODE_AUTO; eventLogAdd("MODE", "AUTO"); }
  else if (mode == "on") { manualMode = MODE_MANUAL_ON; eventLogAdd("MODE", "MANUAL ON"); }
  else if (mode == "off") { manualMode = MODE_MANUAL_OFF; eventLogAdd("MODE", "MANUAL OFF"); }
  else if (mode == "maint_on") { maintenanceMode = true; eventLogAdd("MODE", "MAINTENANCE ON"); }
  else if (mode == "maint_off") { maintenanceMode = false; eventLogAdd("MODE", "MAINTENANCE OFF"); }
  server.send(200, "text/plain", "OK");
}

static void handleNotifySave() {
  if (!requireAuth()) return;
  notificationConfigSave(
    server.arg("telegramEnable") == "1",
    server.arg("telegramToken"),
    server.arg("telegramChatId"),
    server.arg("lineEnable") == "1",
    server.arg("lineToken"),
    server.arg("emailEnable") == "1",
    server.arg("smtpHost"),
    server.hasArg("smtpPort") ? server.arg("smtpPort").toInt() : 587,
    server.arg("smtpUser"),
    server.arg("smtpPass"),
    server.arg("emailTo")
  );
  eventLogAdd("NOTIFY", "Notification config saved");
  server.send(200, "text/plain", "OK");
}

static void handleNotifyTest() {
  if (!requireAuth()) return;
  notifyAlarm("TEST", "ทดสอบระบบแจ้งเตือนจาก Dashboard");
  server.send(200, "text/plain", "SENT");
}

static void handleExportConfig() {
  if (!requireAuth()) return;
  eventLogAdd("CONFIG", "Configuration exported");
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
  eventLogAdd("CONFIG", ok ? "Configuration imported" : "Configuration import failed");
  server.send(ok ? 200 : 400, "text/plain", ok ? "IMPORTED" : "INVALID JSON");
}

static void handleHealth() { server.send(200, "application/json", healthJson()); }
static void handleSystem() { server.send(200, "application/json", systemMonitorJson()); }
static void handleGraph() { server.send(200, "application/json", graphHistoryJson()); }

static void handleOtaInfo() {
  if (!requireAuth()) return;
  server.send(200, "application/json", otaPartitionInfoJson());
}

static void handleFirmwareUploadDone() {
  if (!requireAuth()) return;
  bool ok = !Update.hasError();
  eventLogAdd("OTA", ok ? "Firmware uploaded successfully" : "Firmware upload failed");
  server.send(ok ? 200 : 500, "text/plain", ok ? "Update OK. Rebooting..." : "Update Failed");
  if (ok) { delay(1000); ESP.restart(); }
}

static void handleFirmwareUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    eventLogAdd("OTA", "Upload started: " + upload.filename);
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) Update.printError(Serial);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) Update.printError(Serial);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (!Update.end(true)) Update.printError(Serial);
  }
}

static void handleEventsCsv() {
  if (!requireAuth()) return;
  server.sendHeader("Content-Disposition", "attachment; filename=events.csv");
  server.send(200, "text/csv", eventLogReadCsv());
}

static void handleEventsJson() {
  if (!requireAuth()) return;
  uint16_t limit = server.hasArg("limit") ? server.arg("limit").toInt() : 100;
  server.send(200, "application/json", eventLogReadJson(limit));
}

static void handleEventsClear() {
  if (!requireAuth()) return;
  eventLogClear();
  eventLogAdd("LOG", "Event log cleared");
  server.send(200, "text/plain", "CLEARED");
}

static void handleRestart() {
  if (!requireAuth()) return;
  eventLogAdd("SYSTEM", "Restart requested from dashboard");
  server.send(200, "text/plain", "Restarting");
  delay(500);
  ESP.restart();
}

static void handleRoot() {
  String html = R"HTML(
<!doctype html><html lang="th"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>ระบบเช็คน้ำขาด by ช่างหนึ่ง</title>
<style>
:root{--bg:#f4f7fb;--card:#fff;--text:#222;--primary:#0b65c2;--danger:#c62828;--muted:#666}
body.dark{--bg:#111827;--card:#1f2937;--text:#e5e7eb;--muted:#9ca3af}
body{font-family:Arial,sans-serif;background:var(--bg);margin:0;color:var(--text)}
.wrap{display:flex;min-height:100vh}.side{width:220px;background:#0f172a;color:white;padding:18px;box-sizing:border-box}
.side h2{font-size:18px}.side a{display:block;color:#cbd5e1;text-decoration:none;padding:9px 0}
.main{flex:1;padding:20px}.card{background:var(--card);padding:16px;border-radius:12px;margin-bottom:12px;box-shadow:0 2px 8px #0002}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:10px}
.value{font-size:24px;font-weight:bold}.ok{background:#e9ffe9;color:#102a10}.alarm{background:#ffe5e5;color:#3a0909;border:1px solid #ff8888}
.warn{background:#fff5d6;color:#3a2c00;border:1px solid #e0b030}input,button,textarea{padding:9px;margin:4px;width:100%;box-sizing:border-box}
button{border:0;border-radius:8px;background:var(--primary);color:#fff;cursor:pointer}.danger{background:var(--danger)}.gray{background:#555}
textarea{height:140px}table{border-collapse:collapse;width:100%}td,th{border:1px solid #ddd;padding:6px;font-size:13px}.muted{color:var(--muted)}
.chart{width:100%;height:260px;border:1px solid #ddd;background:#fff;border-radius:8px}
@media(max-width:800px){.wrap{display:block}.side{width:100%}}
</style></head><body>
<div class="wrap">
<div class="side"><h2>WaterCheck</h2><a href="#home">Home</a><a href="#graph">Graph</a><a href="#config">Config</a><a href="#notify">Notify</a><a href="#control">Control</a><a href="#log">Event Log</a><a href="#ota">OTA</a><a href="#health">Health</a><button onclick="toggleDark()">Dark Mode</button></div>
<div class="main">
<h1>ระบบเช็คน้ำขาด by ช่างหนึ่ง</h1>
<div id="banner" class="card"></div>
<div id="home" class="grid">
<div class="card"><small>Flow</small><div id="flow" class="value">-</div></div>
<div class="card"><small>Pump</small><div id="pump" class="value">-</div></div>
<div class="card"><small>State</small><div id="state" class="value">-</div></div>
<div class="card"><small>Countdown</small><div id="remain" class="value">-</div></div>
<div class="card"><small>Retry</small><div id="retry" class="value">-</div></div>
<div class="card"><small>WiFi</small><div id="wifi" class="value">-</div></div>
</div>
<div id="graph" class="card"><h2>Graph Dashboard</h2><canvas id="graphCanvas" class="chart" width="900" height="260"></canvas></div>
<div id="health" class="card"><h2>Health / System Monitor</h2><pre id="healthText"></pre><pre id="systemText"></pre></div>
<div id="config" class="card">
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
<div id="notify" class="card">
<h2>Notification</h2>
<label><input id="telegramEnable" type="checkbox"> Enable Telegram</label>
<label>Telegram Bot Token<input id="telegramToken"></label>
<label>Telegram Chat ID<input id="telegramChatId"></label>
<label><input id="lineEnable" type="checkbox"> Enable LINE compatible sender</label>
<label>LINE Token<input id="lineToken"></label>
<label><input id="emailEnable" type="checkbox"> Enable Email SMTP config</label>
<label>SMTP Host<input id="smtpHost"></label>
<label>SMTP Port<input id="smtpPort" type="number" value="587"></label>
<label>SMTP User<input id="smtpUser"></label>
<label>SMTP Password<input id="smtpPass" type="password"></label>
<label>Email To<input id="emailTo"></label>
<button onclick="saveNotify()">Save Notification</button>
<button onclick="testNotify()">Test Notification</button>
<pre id="notifyStatus"></pre>
</div>
<div class="card">
<h2>Backup / Restore Config</h2>
<button onclick="location.href='/api/config/export'">Download Config JSON</button>
<textarea id="importBox" placeholder="วาง JSON config ที่ต้องการ Restore"></textarea>
<button onclick="importConfig()">Import Config JSON</button>
</div>
<div id="ota" class="card">
<h2>OTA Firmware Update</h2>
<form method="POST" action="/api/ota/upload" enctype="multipart/form-data">
<input type="file" name="firmware" accept=".bin"><button type="submit">Upload Firmware .bin</button></form>
<button onclick="otaInfo()">Show OTA Partition Info</button><pre id="otaText"></pre>
</div>
<div id="control" class="card">
<h2>Control</h2>
<button onclick="mode('auto')">AUTO</button><button onclick="mode('on')">Manual Pump ON</button><button onclick="mode('off')">Manual Pump OFF</button>
<button onclick="mode('maint_on')" class="gray">Maintenance ON</button><button onclick="mode('maint_off')" class="gray">Maintenance OFF</button>
<button onclick="unlock()" class="danger">Unlock / Reset Alarm</button><button onclick="restart()" class="danger">Restart ESP32</button>
</div>
<div id="log" class="card">
<h2>Event Log</h2>
<button onclick="location.href='/api/events.csv'">Download CSV</button><button onclick="loadEvents()">Refresh Log</button><button class="danger" onclick="clearEvents()">Clear Log</button>
<div id="events"></div>
</div>
<div class="card"><h2>About</h2><pre id="event"></pre><p class="muted">Firmware footer: <span id="footerVersion"></span></p></div>
</div></div>
<script>
function stateName(n){return ['RUN','WAIT','TEST','LOCKOUT','ESTOP','MAINT'][n]||'UNK'}
function toggleDark(){document.body.classList.toggle('dark');localStorage.dark=document.body.classList.contains('dark')?'1':'0'}
if(localStorage.dark==='1')document.body.classList.add('dark')
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
 healthText.textContent=JSON.stringify(j.health,null,2); systemText.textContent=JSON.stringify(j.system,null,2);
 event.textContent='Version: '+j.version+'\\nBuild: '+j.build+'\\nIP: '+j.ip+'\\nMQTT: '+j.mqtt+'\\nManualMode: '+j.manualMode+'\\nLastEvent: '+j.lastEvent+'\\nLogSize: '+j.eventLogSize+' bytes\\nLastAlarm: '+j.lastAlarm+'\\nRestart: '+j.restart;
 footerVersion.textContent=j.version+' / '+j.build;
}
async function save(){const fd=new FormData();['delay','test','retrySet','relayFailSec','mqttServer','mqttPort','mqttUser','mqttPass','mqttTopic'].forEach(id=>fd.append(id=='retrySet'?'retry':id,document.getElementById(id).value));await fetch('/api/save',{method:'POST',body:fd});alert('Saved')}
async function unlock(){await fetch('/api/unlock',{method:'POST'});load()}
async function mode(m){const fd=new FormData();fd.append('mode',m);await fetch('/api/mode',{method:'POST',body:fd});load()}
async function restart(){if(confirm('Restart ESP32?')) await fetch('/api/restart',{method:'POST'});}
async function importConfig(){let txt=importBox.value;let r=await fetch('/api/config/import',{method:'POST',headers:{'Content-Type':'application/json'},body:txt});alert(await r.text());}
async function otaInfo(){let r=await fetch('/api/ota/info');otaText.textContent=await r.text();}
async function loadEvents(){let r=await fetch('/api/events.json?limit=150');let arr=await r.json();let html='<table><tr><th>Time</th><th>Type</th><th>Message</th></tr>';arr.forEach(e=>html+='<tr><td>'+e.timestamp+'</td><td>'+e.type+'</td><td>'+e.message+'</td></tr>');html+='</table>';events.innerHTML=html;}
async function clearEvents(){if(confirm('Clear event log?')){await fetch('/api/events/clear',{method:'POST'});loadEvents();}}
async function saveNotify(){const fd=new FormData();['telegramToken','telegramChatId','lineToken','smtpHost','smtpPort','smtpUser','smtpPass','emailTo'].forEach(id=>fd.append(id,document.getElementById(id).value));fd.append('telegramEnable',telegramEnable.checked?'1':'0');fd.append('lineEnable',lineEnable.checked?'1':'0');fd.append('emailEnable',emailEnable.checked?'1':'0');await fetch('/api/notify/save',{method:'POST',body:fd});alert('Saved')}
async function testNotify(){let r=await fetch('/api/notify/test',{method:'POST'});alert(await r.text())}
async function loadNotify(){let r=await fetch('/api/notify/status');notifyStatus.textContent=await r.text();}
function drawGraph(arr){const c=graphCanvas,ctx=c.getContext('2d');ctx.clearRect(0,0,c.width,c.height);ctx.strokeStyle='#ddd';ctx.beginPath();for(let y=40;y<240;y+=50){ctx.moveTo(40,y);ctx.lineTo(880,y)}ctx.stroke();function line(key,color,ybase,scale){ctx.strokeStyle=color;ctx.lineWidth=2;ctx.beginPath();arr.forEach((p,i)=>{let x=40+i*(840/Math.max(1,arr.length-1));let y=ybase-(p[key]*scale);if(i==0)ctx.moveTo(x,y);else ctx.lineTo(x,y)});ctx.stroke()}line('pump','#0b65c2',220,60);line('flow','#16a34a',160,60);line('alarm','#c62828',100,60);ctx.fillStyle='#111';ctx.fillText('Pump blue / Flow green / Alarm red',50,20)}
async function loadGraph(){let r=await fetch('/api/graph');let arr=await r.json();drawGraph(arr)}
setInterval(load,1000);setInterval(loadGraph,5000);load();loadEvents();loadGraph();loadNotify();
</script></body></html>
)HTML";
  server.send(200, "text/html; charset=utf-8", html);
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
  server.on("/api/system", HTTP_GET, handleSystem);
  server.on("/api/graph", HTTP_GET, handleGraph);

  server.on("/api/notify/status", HTTP_GET, []() {
    if (!requireAuth()) return;
    server.send(200, "application/json", notifyStatusJson());
  });
  server.on("/api/notify/save", HTTP_POST, handleNotifySave);
  server.on("/api/notify/test", HTTP_POST, handleNotifyTest);

  server.on("/api/config/export", HTTP_GET, handleExportConfig);
  server.on("/api/config/import", HTTP_POST, handleImportConfig);
  server.on("/api/ota/info", HTTP_GET, handleOtaInfo);
  server.on("/api/ota/upload", HTTP_POST, handleFirmwareUploadDone, handleFirmwareUpload);

  server.on("/api/events.csv", HTTP_GET, handleEventsCsv);
  server.on("/api/events.json", HTTP_GET, handleEventsJson);
  server.on("/api/events/clear", HTTP_POST, handleEventsClear);

  server.begin();
}

void dashboardLoop() {
  server.handleClient();
}
