#include <Arduino.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MQTT_OTA.hpp>

MqttConnector *mqtt;

#define MQTT_HOST         "mqtt.cmmc.io"
#define MQTT_PORT         2883
#define MQTT_USERNAME     ""
#define MQTT_PASSWORD     ""
#define MQTT_CLIENT_ID    ""
#define MQTT_PREFIX       "/CMMC"
#define PUBLISH_EVERY     (500)// every 10 seconds

/* DEVICE DATA & FREQUENCY */
#define DEVICE_NAME       "BASIC-CMMC-001"

/* WIFI INFO */
#ifndef WIFI_SSID
  #define WIFI_SSID        "ESPERT-3020"
  #define WIFI_PASSWORD    "espertap"
#endif

#include <Ticker.h>

bool door_open = 1;

#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"
#include "RunningAverage.h"

Ticker ticker;
Ticker ticker2;

int cached_adc = 0;
int prev_diff = 0;
int current_adc = 0;


RunningAverage bucket(50);

void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  bucket.clear(); // explicitly start cleans

  cached_adc = analogRead(A0);
  Serial.println();
  Serial.println("Serial port initialized.");
  ticker.attach_ms(5, []() {
    current_adc = analogRead(A0);
  });

  ticker2.attach_ms(1000, []() {
    int diff = cached_adc - current_adc;
    bucket.addValue(diff);
    cached_adc = current_adc;
    // Serial.printf("diff = %d, average = %d \r\n", diff, (int) bucket.getAverage());
    if (abs(diff) > 10) {
      if (diff > 0) {
        Serial.println("CLOSED!");
        door_open = 0;
      }
      else if (diff < 0) {
        Serial.println("OPENNED!");
        door_open = 1;
      }
      // Serial.println (diff/ (prev_diff+1));
    }

  });
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

  delay(200);
  init_mqtt();
}

void loop()
{
  mqtt->loop();
}
