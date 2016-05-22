#include <Arduino.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <DHT.h>


#define DHTPIN 12
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

MqttConnector *mqtt;

#define MQTT_HOST         "mqtt.espert.io"
#define MQTT_PORT         1883
#define MQTT_USERNAME     ""
#define MQTT_PASSWORD     ""
#define MQTT_CLIENT_ID    ""
#define MQTT_PREFIX       "/CMMC"
#define PUBLISH_EVERY     (5*1000)// every 5 seconds

/* DEVICE DATA & FREQUENCY */
#define DEVICE_NAME       "NAT-NEOPIXEL-001"
/* WIFI INFO */
#ifndef WIFI_SSID
  #define WIFI_SSID        ""
  #define WIFI_PASSWORD    ""
#endif

Adafruit_NeoPixel pixel(1 , 2 , NEO_GRB + NEO_KHZ800);

#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"

void init_hardware()
{
  Serial.begin(115200);
  pixel.begin();
  pixel.show();
  delay(100);
  pixel.setPixelColor(0, pixel.Color(255, 0, 0));
  Serial.println("OK..");

  // for(uint16_t i=0; i<pixel.numPixels(); i++) {
  //   pixel.show();
  //   delay(20);
  // }

  delay(10);
  dht.begin();
  Serial.println();
  Serial.println("BEGIN");
}

void setup()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.println("CONNECTING...");
    delay(300);
  }
  Serial.println("..WIFI CONNECTED..");
  delay(1000);
  init_hardware();
  init_mqtt();
}

void loop()
{
  mqtt->loop();
}
