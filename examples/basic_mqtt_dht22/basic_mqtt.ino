#include <Arduino.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN 12
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

MqttConnector *mqtt;


#define MQTT_PORT        1883
#define PUBLISH_EVERY    3*1000 // every 15 seconds

/* comment ทิ้งถ้าไม่ส่ username/password */


#define MQTT_HOST     "mqtt.espert.io"
// #define MQTT_USERNAME "z7KXagr95sny6Zx"
// #define MQTT_PASSWORD "jPxRZFXfoshDxLHk79ChVFLu5/8="
#define MQTT_CLIENT_ID "OHXceTmPuUaYt41A"
#define MQTT_PREFIX ""

/* SENSOR INFO */
#define DEVICE_NAME "NAT-001"
#define AUTHOR      "Nat Weerawan"
#define BOARD       "ESPresso Lite V2"
#define SENSOR      "DHT22"

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
  dht.begin();
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
