#include "Dashboard.h"
#include "global.h"
#include "config.h"
#include "Storage.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

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
  StaticJsonDocument<512> doc;
  doc["title"] = DASHBOARD_TITLE;
  doc["version"] = FIRMWARE_VERSION;
  doc["flow"] = flowState;
  doc["pump"] = pumpRelayState;
  doc["state"] = (int)pumpState;
  doc["alarm"] = alarmActive;
  doc["retry"] = currentRetryCount;
  doc["maxRetry"] = maxRetryCount;
  doc["delay"] = delayTimeSec;
  doc["test"] = pumpTestTimeSec;
  doc["remain"] = remainTimeSec;
  doc["mqtt"] = mqttConnected;
  doc["lastEvent"] = lastEvent;
  doc["lastFlowStop"] = lastFlowStopTime;
  doc["lastPumpTest"] = lastPumpTestTime;
  doc["lastFlowReturn"] = lastFlowReturnTime;
  doc["lastAlarm"] = lastAlarmTime;

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

static void handleSave() {
  if (!requireAuth()) return;

  if (server.hasArg("delay")) storageSaveDelay(server.arg("delay").toInt());
  if (server.hasArg("test")) storageSavePumpTest(server.arg("test").toInt());
  if (server.hasArg("retry")) storageSaveMaxRetry((uint8_t)server.arg("retry").toInt());

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
  currentRetryCount = 0;
  lastEvent = "Unlock from dashboard";
  pumpState = PUMP_DELAY_WAIT;
  stateStartMs = millis();
  remainTimeSec = delayTimeSec;
  server.send(200, "text/plain", "UNLOCKED");
}

static void handleRoot() {
  String html = R"HTML(
<!doctype html>
<html lang="th">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ระบบเช็คน้ำขาด by ช่างหนึ่ง</title>
<style>
body{font-family:Arial,sans-serif;margin:20px;background:#f4f7fb;color:#222}
.card{background:#fff;padding:16px;border-radius:12px;margin-bottom:12px;box-shadow:0 2px 8px #0001}
h1{font-size:24px}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(160px,1fr));gap:10px}
.value{font-size:28px;font-weight:bold}
.alarm{background:#ffe5e5;border:1px solid #ff9b9b}
.ok{background:#e9ffe9;border:1px solid #9dd99d}
label{display:block;margin-top:8px}
input{width:100%;padding:8px;margin-top:4px;box-sizing:border-box}
button{padding:10px 14px;border:0;border-radius:8px;background:#0b65c2;color:white;margin-top:12px;cursor:pointer}
button.danger{background:#c62828}
small{color:#666}
</style>
</head>
<body>
<h1>ระบบเช็คน้ำขาด by ช่างหนึ่ง</h1>
<div id="alarmBox" class="card"></div>
<div class="grid">
<div class="card"><small>Flow</small><div id="flow" class="value">-</div></div>
<div class="card"><small>Pump</small><div id="pump" class="value">-</div></div>
<div class="card"><small>State</small><div id="state" class="value">-</div></div>
<div class="card"><small>Countdown</small><div id="remain" class="value">-</div></div>
<div class="card"><small>Retry</small><div id="retry" class="value">-</div></div>
<div class="card"><small>MQTT</small><div id="mqtt" class="value">-</div></div>
</div>
<div class="card">
<h2>Config</h2>
<label>Delay Time (sec)<input id="delay" type="number"></label>
<label>Pump Test Time (sec)<input id="test" type="number"></label>
<label>Max Retry Count<input id="maxRetry" type="number"></label>
<label>MQTT Server<input id="mqttServer"></label>
<label>MQTT Port<input id="mqttPort" type="number" value="1883"></label>
<label>MQTT User<input id="mqttUser"></label>
<label>MQTT Password<input id="mqttPass" type="password"></label>
<label>MQTT Topic<input id="mqttTopic" value="water/check"></label>
<button onclick="save()">Save Config</button>
<button class="danger" onclick="unlock()">Unlock / Reset Alarm</button>
</div>
<div class="card"><h2>Event</h2><pre id="event"></pre></div>
<script>
function stateName(n){return ['RUN','WAIT','TEST','LOCKOUT'][n]||'UNK'}
async function load(){
 const r=await fetch('/api/status'); const j=await r.json();
 flow.textContent=j.flow?'ON':'OFF'; pump.textContent=j.pump?'ON':'OFF';
 state.textContent=stateName(j.state); remain.textContent=j.remain+'s';
 retry.textContent=j.retry+'/'+j.maxRetry; mqtt.textContent=j.mqtt?'OK':'OFF';
 delay.value=j.delay; test.value=j.test; maxRetry.value=j.maxRetry;
 alarmBox.className='card '+(j.alarm?'alarm':'ok');
 alarmBox.innerHTML=j.alarm?'<h2>ALARM / LOCKOUT</h2><p>น้ำยังไม่กลับมาหลังทดลองเดินปั๊มครบจำนวนครั้ง</p>':'<h2>System OK</h2>';
 event.textContent='Last: '+j.lastEvent+'\\nFlow Stop: '+j.lastFlowStop+'\\nPump Test: '+j.lastPumpTest+'\\nFlow Return: '+j.lastFlowReturn+'\\nAlarm: '+j.lastAlarm;
}
async function save(){
 const fd=new FormData();
 ['delay','test','maxRetry','mqttServer','mqttPort','mqttUser','mqttPass','mqttTopic'].forEach(id=>fd.append(id,document.getElementById(id).value));
 await fetch('/api/save',{method:'POST',body:fd});
 alert('Saved');
}
async function unlock(){ await fetch('/api/unlock',{method:'POST'}); load(); }
setInterval(load,1000); load();
</script>
</body></html>
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
  server.begin();
}
