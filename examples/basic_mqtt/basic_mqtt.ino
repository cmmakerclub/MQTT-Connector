#include <Arduino.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

MqttConnector *mqtt;

/* comment ทิ้งถ้าไม่ส่ username/password */

#define MQTT_HOST     "mqtt.espert.io"
#define MQTT_PORT        1883
/* AUTH */
/****************************************************
 #define MQTT_USERNAME "z7KXagr95sny6Zx"
 #define MQTT_PASSWORD "jPxRZFXfoshDxLHk79ChVFLu5/8="
***************************************************/
#define MQTT_CLIENT_ID "OHXceTmPuUaYt41A"
#define MQTT_PREFIX ""
#define PUBLISH_EVERY    (10*1000)// every 10 seconds

/* SENSOR INFO */
#define DEVICE_NAME "DEEPSLEEP-001"

/* WIFI INFO */
#ifndef WIFI_SSID
  #define WIFI_SSID        ""
  #define WIFI_PASSPHARSE  ""
#endif

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
  WiFi.begin("Nat", "123456789");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.println("CONNECTING...");
    delay(300);
  }
  init_hardware();
  init_mqtt();
}

void loop()
{
  mqtt->loop();
}
