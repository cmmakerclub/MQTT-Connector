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

String MQTT_HOST        = "mqtt.cmmc.io";
String MQTT_USERNAME    = "";
String MQTT_PASSWORD    = "";
String MQTT_CLIENT_ID   = "";
String MQTT_PREFIX      = "CMMC";
int    MQTT_PORT        = 1883;

int PUBLISH_EVERY       = 1000;

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
