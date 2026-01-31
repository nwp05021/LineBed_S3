#include "services/MqttService.h"
#include "services/SettingsStore.h"
#include "services/PendingSettings.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "config/Config.h"

#if defined(USE_AWS_IOT)
  #include <WiFiClientSecure.h>
  #include "config/Secrets.h"
  static WiFiClientSecure net;
#else
  static WiFiClient net;
#endif

static PubSubClient mqtt(net);
static const char* MQTT_HOST = "test.mosquitto.org";
static const uint16_t MQTT_PORT = 1883;

static MqttService* gSelf = nullptr;
static void mqttCallback(char* topic, uint8_t* payload, unsigned int len) {
  if (gSelf) gSelf->handleMessage(topic, payload, len);
}

#if defined(USE_AWS_IOT)
#ifndef AWS_THING_NAME
#define AWS_THING_NAME DEVICE_LINE_ID "-" DEVICE_BED_ID
#endif
static String shadowBase() { return String("$aws/things/") + AWS_THING_NAME + "/shadow"; }
static String tShadowUpdate() { return shadowBase() + "/update"; }
static String tShadowUpdateAccepted() { return shadowBase() + "/update/accepted"; }
static String tShadowUpdateDelta() { return shadowBase() + "/update/delta"; }
static String tShadowGet() { return shadowBase() + "/get"; }
static String tShadowGetAccepted() { return shadowBase() + "/get/accepted"; }
#endif

static bool parseSettingsJson(const JsonVariantConst& j, PendingSettings& ps) {
  bool any = false;
  if (j.containsKey("setTemp10")) {
    ps.hasSetTemp10 = true;
    ps.setTemp10 = j["setTemp10"].as<int>();
    any = true;
  }
  if (j.containsKey("setHumid")) { ps.hasSetHumid = true; ps.setHumid = j["setHumid"].as<int>(); any = true; }
  if (j.containsKey("ledOnSec")) { ps.hasLedOn = true; ps.ledOnSec = j["ledOnSec"].as<uint16_t>(); any = true; }
  if (j.containsKey("ledOffSec")){ ps.hasLedOff = true; ps.ledOffSec = j["ledOffSec"].as<uint16_t>(); any = true; }
  ps.hasAny = any;
  return any;
}

void MqttService::begin(SettingsStore& settings) {
  _settings = &settings;
  static PendingSettings incoming;
  incoming.clear();
  _incoming = &incoming;
  _hasIncoming = false;

  gSelf = this;

#if defined(USE_AWS_IOT)
  net.setCACert(AWS_ROOT_CA_PEM);
  net.setCertificate(AWS_DEVICE_CERT_PEM);
  net.setPrivateKey(AWS_PRIVATE_KEY_PEM);
  mqtt.setServer(AWS_IOT_ENDPOINT, AWS_IOT_PORT);
#else
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
#endif
  mqtt.setCallback(mqttCallback);
}

bool MqttService::isConnected() const { return _connected && mqtt.connected(); }

bool MqttService::popIncomingSettings(PendingSettings& out) {
  if (!_hasIncoming) return false;
  out = *_incoming;
  _incoming->clear();
  _hasIncoming = false;
  return true;
}

void MqttService::publishReportedNow() {
#if defined(USE_AWS_IOT)
  publishShadowReported();
#endif
}

void MqttService::tick(uint32_t nowMs, bool wifiConnected) {
  if (!wifiConnected) { _connected = false; return; }

  if (!mqtt.connected()) {
    _connected = false;
    if ((int32_t)(nowMs - _nextRetryMs) < 0) return;
    _nextRetryMs = nowMs + 5000;

    String cid = String("sf-") + DEVICE_LINE_ID + "-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    Serial.printf("[MQTT] connect %s...\n", cid.c_str());

    if (mqtt.connect(cid.c_str())) {
      _connected = true;
#if defined(USE_AWS_IOT)
      Serial.println("[MQTT] connected (AWS IoT)");
      mqtt.subscribe(tShadowUpdateDelta().c_str());
      mqtt.subscribe(tShadowGetAccepted().c_str());
      mqtt.subscribe(tShadowUpdateAccepted().c_str());
      requestShadowGet();
#else
      Serial.println("[MQTT] connected");
      mqtt.subscribe(topicCmd().c_str());
      mqtt.subscribe(topicCmdSet().c_str());
      mqtt.subscribe(topicCmdReboot().c_str());
#endif
    } else {
      Serial.printf("[MQTT] failed rc=%d\n", mqtt.state());
      return;
    }
  }

  mqtt.loop();

#if !defined(USE_AWS_IOT)
  static uint32_t nextPub = 0;
  if ((int32_t)(nowMs - nextPub) >= 0) {
    nextPub = nowMs + 5000;

    StaticJsonDocument<256> doc;
    doc["ts"] = (uint32_t)(nowMs/1000);
    doc["setTemp10"] = _settings->getSetTemp10();
    doc["setHumid"] = _settings->getSetHumid();
    doc["ledOnSec"] = _settings->getLedOnSec();
    doc["ledOffSec"] = _settings->getLedOffSec();

    char buf[256];
    size_t n = serializeJson(doc, buf, sizeof(buf));
    mqtt.publish(topicState().c_str(), (uint8_t*)buf, n);
  }
#endif
}

void MqttService::handleMessage(char* topic, uint8_t* payload, unsigned int len) {
  String t(topic);
  String s;
  s.reserve(len+1);
  for (unsigned int i=0;i<len;i++) s += (char)payload[i];

  Serial.printf("[MQTT] msg topic=%s len=%u\n", topic, len);

#if defined(USE_AWS_IOT)
  if (t == tShadowUpdateDelta() || t == tShadowGetAccepted() || t == tShadowUpdateAccepted()) {
    StaticJsonDocument<768> doc;
    if (deserializeJson(doc, s)) return;

    PendingSettings ps;
    ps.clear();

    if (t == tShadowUpdateDelta()) {
      if (parseSettingsJson(doc["state"], ps)) { *_incoming = ps; _hasIncoming = true; }
    } else {
      JsonVariantConst desired = doc["state"]["desired"];
      if (!desired.isNull()) {
        if (parseSettingsJson(desired, ps)) { *_incoming = ps; _hasIncoming = true; }
      }
    }
    return;
  }
#else
  if (t == topicCmdReboot()) {
    Serial.println("[MQTT] reboot requested");
    delay(50);
    ESP.restart();
    return;
  }
  if (t == topicCmdSet() || t == topicCmd()) {
    StaticJsonDocument<256> doc;
    if (deserializeJson(doc, s)) return;
    PendingSettings ps;
    ps.clear();
    if (parseSettingsJson(doc.as<JsonVariantConst>(), ps)) { *_incoming = ps; _hasIncoming = true; }
    return;
  }
#endif
}

void MqttService::requestShadowGet() {
#if defined(USE_AWS_IOT)
  StaticJsonDocument<16> doc;
  char buf[16];
  size_t n = serializeJson(doc, buf, sizeof(buf));
  mqtt.publish(tShadowGet().c_str(), (uint8_t*)buf, n);
#endif
}

void MqttService::publishShadowReported() {
#if defined(USE_AWS_IOT)
  StaticJsonDocument<256> doc;
  JsonObject rep = doc["state"]["reported"].to<JsonObject>();
  rep["setTemp10"] = _settings->getSetTemp10();
  rep["setHumid"] = _settings->getSetHumid();
  rep["ledOnSec"] = _settings->getLedOnSec();
  rep["ledOffSec"] = _settings->getLedOffSec();

  char buf[256];
  size_t n = serializeJson(doc, buf, sizeof(buf));
  mqtt.publish(tShadowUpdate().c_str(), (uint8_t*)buf, n);
#endif
}
