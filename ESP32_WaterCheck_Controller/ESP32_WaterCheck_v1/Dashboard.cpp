#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "global.h"
#include "config.h"
#include "Storage.h"
#include "Relay.h"
#include "MQTTClient.h"
#include "Dashboard.h"

static WebServer server(80);
static bool isAuth() { return server.authenticate(settings.adminUser.c_str(), settings.adminPass.c_str()); }
static void requireAuth() { server.requestAuthentication(BASIC_AUTH, "ESP32 WaterCheck", "Login required"); }

static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!doctype html><html lang="th"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>ระบบเช็คน้ำขาด</title><style>
body{font-family:Arial,'Noto Sans Thai',sans-serif;margin:0;background:#eef3f8;color:#14213d}.wrap{max-width:980px;margin:auto;padding:18px}.card{background:white;border-radius:16px;padding:18px;margin:12px 0;box-shadow:0 4px 18px #0001}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(160px,1fr));gap:12px}.box{background:#f7fafc;border-radius:12px;padding:14px}.v{font-size:24px;font-weight:700}.ok{color:#16883f}.bad{color:#b00020}input,button{font-size:16px;padding:10px;border-radius:8px;border:1px solid #ccd}button{cursor:pointer;background:#1565c0;color:white;border:0}.row{display:flex;gap:8px;flex-wrap:wrap}label{display:block;margin-top:10px;font-weight:700}small{color:#555}</style></head><body><div class="wrap"><h1>ระบบเช็คน้ำขาด by ช่างหนึ่ง</h1><div class="card"><div class="grid"><div class="box">เวลา<div id="time" class="v">--</div></div><div class="box">Flow<div id="flow" class="v">--</div></div><div class="box">Pump<div id="pump" class="v">--</div></div><div class="box">Countdown<div id="remain" class="v">--</div></div><div class="box">MQTT<div id="mqtt" class="v">--</div></div><div class="box">WiFi<div id="wifi" class="v">--</div></div></div></div><div class="card"><h2>Config</h2><small>เมนูนี้มี Basic Auth: ค่าเริ่มต้น admin / 123456</small><label>Delay หลัง Flow หยุด (วินาที)</label><div class="row"><input id="delay" type="number" min="1" max="86400"><button onclick="saveDelay()">บันทึก Delay</button></div><h3>MQTT</h3><label>Server</label><input id="ms" placeholder="192.168.1.10"><label>Port</label><input id="mp" type="number" value="1883"><label>User</label><input id="mu"><label>Password</label><input id="mw" type="password"><label>Base Topic</label><input id="mt" value="watercheck"><br><br><button onclick="saveMqtt()">บันทึก MQTT</button><button onclick="cmd('PUMP_ON')">Pump ON</button><button onclick="cmd('PUMP_OFF')">Pump OFF</button><button onclick="restart()">Restart</button></div></div><script>
async function j(u,o){let r=await fetch(u,o);if(!r.ok)throw new Error(r.status);return await r.json()}async function update(){try{let s=await j('/api/status');time.textContent=s.date+' '+s.time;flow.textContent=s.flow?'ON':'OFF';flow.className='v '+(s.flow?'ok':'bad');pump.textContent=s.pump?'ON':'OFF';pump.className='v '+(s.pump?'ok':'bad');remain.textContent=s.counting?s.remain+' s':'Ready';mqtt.textContent=s.mqtt?'Online':'Offline';wifi.textContent=s.rssi+' dBm';delay.value=s.delay;ms.value=s.mqttServer;mp.value=s.mqttPort;mu.value=s.mqttUser;mt.value=s.mqttTopic;}catch(e){console.log(e)}}async function saveDelay(){await j('/api/config/delay',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({delay:Number(delay.value)})});update()}async function saveMqtt(){await j('/api/config/mqtt',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({server:ms.value,port:Number(mp.value),user:mu.value,pass:mw.value,topic:mt.value})});update()}async function cmd(c){await j('/api/cmd',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({cmd:c})});update()}async function restart(){await fetch('/api/restart',{method:'POST'})}setInterval(update,1000);update();
</script></body></html>
)rawliteral";

static void sendStatus() {
  StaticJsonDocument<512> doc;
  doc["flow"] = state.flowOn; doc["pump"] = state.pumpOn; doc["aux"] = state.auxOn;
  doc["counting"] = state.counting; doc["remain"] = state.remainSec; doc["delay"] = settings.delaySec;
  doc["time"] = state.timeText; doc["date"] = state.dateText; doc["ip"] = state.ip; doc["rssi"] = state.rssi;
  doc["mqtt"] = state.mqttConnected; doc["mqttServer"] = settings.mqttServer; doc["mqttPort"] = settings.mqttPort;
  doc["mqttUser"] = settings.mqttUser; doc["mqttTopic"] = settings.mqttBaseTopic; doc["lastEvent"] = state.lastEvent;
  String out; serializeJson(doc, out); server.send(200, "application/json", out);
}

static bool parseJson(StaticJsonDocument<512>& doc) {
  if (!server.hasArg("plain")) return false;
  return deserializeJson(doc, server.arg("plain")) == DeserializationError::Ok;
}

void dashboardBegin() {
  server.on("/", HTTP_GET, [](){ server.send_P(200, "text/html; charset=utf-8", INDEX_HTML); });
  server.on("/api/status", HTTP_GET, sendStatus);
  server.on("/api/config/delay", HTTP_POST, [](){ if(!isAuth()){requireAuth();return;} StaticJsonDocument<512> doc; if(!parseJson(doc)){server.send(400,"application/json","{\"ok\":false}");return;} saveDelay(doc["delay"] | DEFAULT_DELAY_SEC); server.send(200,"application/json","{\"ok\":true}"); });
  server.on("/api/config/mqtt", HTTP_POST, [](){ if(!isAuth()){requireAuth();return;} StaticJsonDocument<512> doc; if(!parseJson(doc)){server.send(400,"application/json","{\"ok\":false}");return;} saveMqtt(doc["server"] | "", doc["port"] | MQTT_DEFAULT_PORT, doc["user"] | "", doc["pass"] | "", doc["topic"] | MQTT_BASE_TOPIC_DEFAULT); mqttBegin(); server.send(200,"application/json","{\"ok\":true}"); });
  server.on("/api/cmd", HTTP_POST, [](){ if(!isAuth()){requireAuth();return;} StaticJsonDocument<512> doc; if(!parseJson(doc)){server.send(400,"application/json","{\"ok\":false}");return;} String cmd=doc["cmd"]|""; if(cmd=="PUMP_ON")setPump(true); else if(cmd=="PUMP_OFF")setPump(false); else if(cmd=="AUX_ON")setAux(true); else if(cmd=="AUX_OFF")setAux(false); server.send(200,"application/json","{\"ok\":true}"); });
  server.on("/api/restart", HTTP_POST, [](){ if(!isAuth()){requireAuth();return;} server.send(200,"application/json","{\"ok\":true}"); delay(300); ESP.restart(); });
  server.begin();
}

void dashboardLoop() { server.handleClient(); }
