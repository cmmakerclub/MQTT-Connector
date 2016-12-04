#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include <MqttConnector.h>

/* WIFI INFO */
#ifndef WIFI_SSID
#define WIFI_SSID        "DEVICES-AP"
#define WIFI_PASSWORD    "devicenetwork"
#endif

#define BLUEMIX_ORG_ID   "r6crrd"
#define BLUEMIX_TYPE_ID  "esp8266" 

String MQTT_HOST        = BLUEMIX_ORG_ID ".messaging.internetofthings.ibmcloud.com";
String MQTT_USERNAME    = "use-token-auth";
String MQTT_PASSWORD    = "blw(@sjhIdis30&gGt";
String MQTT_CLIENT_ID   = "";
String MQTT_PREFIX      = "";
int    MQTT_PORT        = 1883;
int PUBLISH_EVERY       = 2000;

MqttConnector *mqtt;


#include "init_mqtt.h"
#include "_publish.h"
#include "_receive.h"

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
  while (WiFi.status() != WL_CONNECTED) {
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
