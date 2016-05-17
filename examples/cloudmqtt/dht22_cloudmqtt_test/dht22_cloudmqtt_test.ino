#include <DHT.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

MqttConnector *mqtt;

#define DHTPIN 12
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

#define MQTT_HOST         "m20.cloudmqtt.com"
#define MQTT_PORT         19642
#define MQTT_USERNAME     "test"
#define MQTT_PASSWORD     "test"
#define MQTT_CLIENT_ID    "S1E6h0qKLTmjC1Vh"
#define MQTT_PREFIX       "/NatWeerawan2/gearname"
#define PUBLISH_EVERY     (10*1000) // every 10 seconds

/* DEVICE DATA & FREQUENCY */
#define DEVICE_NAME       "CMMC-001"
#define PUBLISH_EVERY     (10*1000)// every 10 seconds

/* WIFI INFO */
#ifndef WIFI_SSID
  #define WIFI_SSID        ""
  #define WIFI_PASSWORD    ""
#endif

#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"

#define DHTPIN 12 // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("BEGIN");

  init_dht();

}

void init_dht() {
  pinMode(DHTPIN, INPUT_PULLUP);
}

void init_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.println("CONNECTING...");
    delay(300);
  }
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
