#include <Arduino.h>

#ifdef ESP8266 
  #include <ESP8266WiFi.h>
#else 
  #include <WiFi.h>
#endif

#include <ArduinoJson.h>
#include <MqttConnector.h>
#include <Wire.h>
#include <SPI.h>

#include "init_mqtt.h"
#include "_publish.h"
#include "_receive.h"
#include "_config.h"

MqttConnector *mqtt;  
#define DHT_PIN 12

DHT dht(DHT_PIN, DHT11);

int relayPinState       = HIGH;
int relayPin            = 15;
int LED_PIN             = 2;

char myName[40];

void init_hardware()
{ 
  pinMode(relayPin, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(DHT_PIN, INPUT_PULLUP);
  digitalWrite(relayPin, relayPinState);

  dht.begin();

  // serial port initialization
  Serial.begin(115200);
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
  digitalWrite(LED_PIN, HIGH);
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
