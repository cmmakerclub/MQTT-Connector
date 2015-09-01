#define MQTT_DEBUG_MODE
#define WIFI_DEBUG_MODE



#include <DHT.h>
#include "dht_helper.h"
#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

MqttConnector *mqtt;
WiFiConnector *wifi;
DHT *dht;


/* MQTT INFO */
#define MQTT_HOST        "rabbit.cmmc.ninja"
#define MQTT_PORT        1883
#define PUBLISH_EVERY    2 *1000 // every 15 seconds

/* comment ทิ้งถ้าไม่ส่ username/password */
#define MQTT_USERNAME    "nat:nat" 
#define MQTT_PASSWORD    "freedom"

/* SENSOR INFO */
#define DEVICE_NAME "DELTA-002"
#define PROJECT     "DELTA"
#define AUTHOR      "Nat Weerawan"
#define BOARD       "esp-mini.v2"
#define SENSOR      "DHT22"

/* WIFI INFO */
#define SSID        "Opendream"
#define PASSPHARSE  "gfkgvkgv'2016!!!!"

#include "init_wifi.h"
#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"

#define DHTPIN 12    // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

void init_hardware()
{
 
  pinMode(DHTPIN, INPUT_PULLUP);
  pinMode(14, OUTPUT);    
  pinMode(13, OUTPUT);    
  digitalWrite(13, HIGH);
  digitalWrite(14, LOW);    

  init_dht(&dht, DHTPIN, DHTTYPE);	
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("BEGIN");
}

void setup()
{
  init_hardware();
  init_wifi();
  init_mqtt();
}

void loop()
{
  mqtt->loop(wifi);
}
