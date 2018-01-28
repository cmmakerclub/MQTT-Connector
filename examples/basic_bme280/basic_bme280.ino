#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttConnector.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>

#include "init_mqtt.h"
#include "_publish.h" 
#include "_receive.h"

MqttConnector *mqtt;

/* WIFI INFO */
String WIFI_SSID        = "ampere";
String WIFI_PASSWORD    = "espertap";

String MQTT_HOST        = "192.168.12.1";
String MQTT_USERNAME    = "";
String MQTT_PASSWORD    = "";
String MQTT_CLIENT_ID   = "";
String MQTT_PREFIX      = "CMMC/";
int    MQTT_PORT        = 1883;
int PUBLISH_EVERY       = 5000;
int MQTT_CONNECT_TIMEOUT= 5000;

String DEVICE_NAME      = "LATTE-001"; 

int relayPinState = HIGH;
int relayPin            = 15; 
char myName[40];


Adafruit_BME280 bme;

void init_hardware()
{
  Wire.begin();
  bme.begin();
  
  pinMode(relayPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(relayPin, relayPinState);;
  // serial port initialization
  Serial.begin(57600);
  delay(10);
  Serial.println();
  Serial.println("Starting...");
}

void init_wifi() {
  WiFi.disconnect();
  delay(20);
  WiFi.mode(WIFI_STA);
  delay(50);
  const char* ssid =  WIFI_SSID.c_str();
  const char* pass =  WIFI_PASSWORD.c_str();
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", ssid, pass);
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
