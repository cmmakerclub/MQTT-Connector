#define MQTT_DEBUG_MODE
#define WIFI_DEBUG_MODE

#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

MqttConnector *mqtt;
WiFiConnector *wifi;

/* MQTT INFO */
#define MQTT_HOST        "rabbit.cmmc.ninja"
#define MQTT_PORT        1883
#define PUBLISH_EVERY    1 *1000 // every 15 seconds

/* comment ทิ้งถ้าไม่ส่ username/password */
#define MQTT_USERNAME    "free:test" 
#define MQTT_PASSWORD    "test"

/* SENSOR INFO */
#define DEVICE_NAME "-RED-"
#define AUTHOR      "Nat Weerawan"
#define BOARD       "dw.mini-esp.v2"
#define PROJECT     "POC"
#define SENSOR      "DHT22"

/* WIFI INFO */
#define SSID        "WIFI SSID"
#define PASSPHARSE  "WIFI PASS"

#include "init_wifi.h"
#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"

void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("BEGIN");
}

void setup()
{
  init_hardware();
  init_wifi();
  init_mqtt();
}

void loop()
{
  mqtt->loop(wifi);
}
