#include <Arduino.h>
#include "CMMC_Blink.hpp"
#include "CMMC_Interval.hpp"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <MqttConnector.h>
#include "init_mqtt.h"
#include <Ticker.h>
#include "_publish.h"
#include "_receive.h"

const char* MQTT_HOST        = "mqtt.espert.io";
const char* MQTT_USERNAME    = "";
const char* MQTT_PASSWORD    = "";
const char* MQTT_CLIENT_ID   = "";
const char* MQTT_PREFIX      = "/CMMC";
const int MQTT_PORT           = 1883;

extern void _publish_run_once();
extern void _publish_run_loop();

extern void _receive_run_once();
extern void _receive_run_loop();

CMMC_Blink blinker;
MqttConnector *mqtt;

/* WIFI INFO */
#ifndef WIFI_SSID
  #define WIFI_SSID        "ESPERT-3020"
  #define WIFI_PASSWORD    "espertap"
#endif


Ticker ticker;



void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("Starting...");
  pinMode(LED_BUILTIN, OUTPUT);
  blinker.init();
  blinker.blink(50, LED_BUILTIN);
  delay(200);

  _publish_run_once();
  _receive_run_once();
}

void setup()
{
  init_hardware();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", WIFI_SSID, WIFI_PASSWORD);
    delay(300);
  }

  Serial.println("WiFi Connected.");
  delay(50);
  blinker.detach();
  digitalWrite(LED_BUILTIN, HIGH);

  init_mqtt();
}

void loop()
{
  mqtt->loop();
  _publish_loop();
  _receive_loop();
}
