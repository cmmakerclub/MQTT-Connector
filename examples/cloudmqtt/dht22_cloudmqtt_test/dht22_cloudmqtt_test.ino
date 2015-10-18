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


/* comment ทิ้งถ้าไม่ส่ username/password */
#define MQTT_USERNAME    "test"
#define MQTT_PASSWORD    "test"
#define MQTT_CLIENT_ID   "gqPFi8OlKxpnw01l"
#define MQTT_PREFIX      "/HelloChiangMaiMakerClub/id"
#define MQTT_HOST        "m20.cloudmqtt.com"
#define MQTT_PORT        19642

#define PUBLISH_EVERY    3*1000 // every 15 seconds

/* SENSOR INFO */
#define DEVICE_NAME "MAKER-ASIA-DHT22-001"
#define AUTHOR      "Nat Weerawan"
#define BOARD       "ESSPRESSO"
#define SENSOR      "DHT22"

/* WIFI INFO */
#define WIFI_SSID        "SSID"
#define WIFI_PASSPHARSE  "TEST"

#include "init_wifi.h"
#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"


//DW-MINI-1
//#define DHTPIN 12 // what pin we're connected to
//#define DHT_VCC 13
//#define DHT_GND 14


//DW-MINI-2
#define DHTPIN 12 // what pin we're connected to
#define DHT_VCC 13
#define DHT_GND 16

//// NODE-MCU-0.9
//#define DHT_VCC 5
//#define DHT_GND 2
//#define DHTPIN 4 // what pin we're connected to


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