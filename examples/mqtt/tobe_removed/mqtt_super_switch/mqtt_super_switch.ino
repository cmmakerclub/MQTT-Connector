#define MQTT_DEBUG_MODE
#define WIFI_DEBUG_MODE

#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

MqttConnector *mqtt;
WiFiConnector *wifi;

int state = true;

#define SWITCH_PIN 0
#define RELAY_PIN  2

// MQTT_HOSTT
#define MQTT_HOST "cmmc.xyz"
#define MQTT_PORT 1883

#include "init_wifi.h"
#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"


void init_hardware()
{
  Serial.begin(9600);
  delay(10);

  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  Serial.println();
  Serial.println("BEGIN");
}

void setup()
{
  init_hardware();
  init_wifi();
  init_mqtt();
}

unsigned long int prev = millis();
long diff ;
void loop()
{
    while (digitalRead(SWITCH_PIN) == HIGH) {
      mqtt->loop(wifi);
      yield();
    }

    while(digitalRead(SWITCH_PIN) == LOW) {
      mqtt->loop(wifi);
      yield();
    }

    // ignore
    if (millis() - prev <= 300) {
        Serial.println(digitalRead(SWITCH_PIN));
    }
    else {
        prev = millis();
        Serial.print("PRESSED.: ");
        mqtt->sync_pub(String(!state));
    }
    mqtt->loop(wifi);

}