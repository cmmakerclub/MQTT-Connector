#include <Arduino.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WiFiConnector.h>
#include <DHT.h>
#include <EEPROM.h>

#define DHTPIN 12
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

#define RELAY_01_PIN 4
#define RELAY_02_PIN 5
#define RELAY_01_MASK 0b01
#define RELAY_02_MASK 0b10
#define EEPROM_ADDR 0x00

bool pin01_state = 0;
bool pin02_state = 0;

MqttConnector *mqtt;

#define MQTT_HOST         "mqtt.espert.io"
#define MQTT_PORT         1883
#define MQTT_USERNAME     ""
#define MQTT_PASSWORD     ""
#define MQTT_CLIENT_ID    ""
#define MQTT_PREFIX       "/CMMC"
#define PUBLISH_EVERY     (5*1000)// every 5 seconds

/* DEVICE DATA & FREQUENCY */
#define DEVICE_NAME       "CNX-PLUG-001"
/* WIFI INFO */
#ifndef WIFI_SSID
  #define WIFI_SSID        ""
  #define WIFI_PASSWORD    ""
#endif



WiFiConnector wifi(WIFI_SSID, WIFI_PASSWORD);

#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"

void init_hardware()
{
  pinMode(DHTPIN, INPUT_PULLUP);

  Serial.begin(115200);
  dht.begin();
  EEPROM.begin(10);
  delay(10);

  delay(10);
  Serial.println();
  Serial.println("BEGIN");

  pinMode(RELAY_01_PIN, OUTPUT);
  pinMode(RELAY_02_PIN, OUTPUT);

  //
  byte val = EEPROM.read(EEPROM_ADDR);
  Serial.println(val, BIN);
  digitalWrite(RELAY_01_PIN, val & RELAY_01_MASK);
  digitalWrite(RELAY_02_PIN, val & RELAY_02_MASK);
}

void init_wifi() {
  wifi.init();

  wifi.on_connected([&](const void* message)
  {
    Serial.print("WIFI CONNECTED WITH IP: ");
    Serial.println(WiFi.localIP());
  });

  wifi.on_connecting([&](const void* message)
  {
    Serial.print("Connecting to ");
    Serial.println(wifi.get("ssid") + ", " + wifi.get("password"));
    delay(200);
  });

  wifi.on_disconnected([&](const void* message) {
    // if (mqtt != NULL) {
    //   delete mqtt;
    //   mqtt = NULL;
    // }
  });
}


void setup()
{
  init_hardware();
  init_mqtt();
  init_wifi();
  wifi.connect();
  mqtt->connect();
}

void loop()
{
  wifi.loop();
  if (wifi.connected()) {
    mqtt->loop();
  }
}
