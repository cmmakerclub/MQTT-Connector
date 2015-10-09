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


#define MQTT_HOST        "rabbit.cmmc.ninja"
#define MQTT_PORT        1883
#define PUBLISH_EVERY    2 *1000 // every 15 seconds

/* comment ทิ้งถ้าไม่ส่ username/password */
#define MQTT_USERNAME    "nat:nat" 
#define MQTT_PASSWORD    "freedom"

/* SENSOR INFO */
#define DEVICE_NAME "NODEMCU-n005"
#define PROJECT     "NODEMCU-SET"
#define AUTHOR      "Nat Weerawan"
#define BOARD       "nodemcu-v0.9"
#define SENSOR      "DHT22"

/* WIFI INFO */
#define SSID        "Opendream"
#define PASSPHARSE  "gfkgvkgv'2016!!!!"

#include "init_wifi.h"
#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"

#define DHT_VCC 5
#define DHT_GND 2
#define DHTPIN 4 // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

void init_hardware()
{

  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("BEGIN");
 
  pinMode(DHTPIN, INPUT_PULLUP);
  pinMode(DHT_VCC, OUTPUT);    
  pinMode(DHT_GND, OUTPUT);    
  digitalWrite(DHT_VCC, HIGH);
  digitalWrite(DHT_GND, LOW);    

  init_dht(&dht, DHTPIN, DHTTYPE);    
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
