#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttConnector.h>
#include "init_mqtt.h"
#include "_publish.h"
#include "_receive.h"

/* WIFI INFO */
#ifndef WIFI_SSID
  #define WIFI_SSID        "DEVICES-AP"
  #define WIFI_PASSWORD    "devicenetwork"
#endif

const char* MQTT_HOST        = "mqtt.cmmc.io";
const char* MQTT_USERNAME    = "";
const char* MQTT_PASSWORD    = "";
const char* MQTT_CLIENT_ID   = "";
const char* MQTT_PREFIX      = "/CMMC";
int   MQTT_PORT              = 1883;

const char* DEVICE_NAME      = "plug001";
int PUBLISH_EVERY            = 3000;

MqttConnector *mqtt;

void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("Starting...");
  pinMode(LED_BUILTIN, OUTPUT);
}

void init_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", WIFI_SSID, WIFI_PASSWORD);
    delay(300);
  }

  Serial.println("WiFi Connected.");
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup()
{
  init_hardware();
  init_wifi();
  init_mqtt();
}

void loop()
{
  mqtt->loop();
}
