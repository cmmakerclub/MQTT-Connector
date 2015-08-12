#define MQTT_DEBUG_MODE
#define WIFI_DEBUG_MODE

#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <pt.h>

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif


const char* ssid     = "NAT.WRTNODE";
const char* pass     = "devicenetwork";

MqttConnector *mqtt;
WiFiConnector *wifi;

void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("BEGIN");
  Serial.println("BEGIN");
  Serial.println("BEGIN");
}

void init_wifi()
{
  wifi = new WiFiConnector(ssid, pass);

  wifi->on_connecting([&](const void* message)
  {
    Serial.print("connecting ");
    Serial.println ((char*)message);
    delay(500);
  });

  wifi->on_connected([&](const void* message)
  {
    Serial.print ("WIFI CONECTED: ");
    Serial.println ((char*)message);
  });

  wifi->on_disconnected([&](const void* message)
  {
    Serial.print ("WIFI DISCONECTED: ");
    Serial.print ("WIFI DISCONECTED: ");
    Serial.print ("WIFI DISCONECTED: ");
    Serial.println ((char*)message);
  });

  wifi->connect();

}

void init_mqtt()
{

  mqtt = new MqttConnector("mqtt.tespa.io");

  mqtt->prepare_configuration([&](MqttConnector::Config * config) -> void {

  });

  mqtt->prepare_configuration(NULL);

  mqtt->prepare_data([&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    // data["myName"] = "0x001";
    data["adc"] = analogRead(A0);
    data["sdk"] = system_get_sdk_version();
  }, 1500);


  mqtt->prepare_subscribe(NULL);
  mqtt->prepare_subscribe([&](MQTT::Subscribe * sub) -> void {
    // (sub)->add_topic("HELLO");
  });


  mqtt->after_prepare_data([&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    //delete data["version"];
  });

  mqtt->on_message([&](const MQTT::Publish & pub) -> void {
    Serial.print("ON MESSAGE: ");
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
  });

  mqtt->on_connecting([&](int count, bool * flag) {
    Serial.print("CONNECTION: ");
    Serial.println(count);
  });

  mqtt->connect();
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
  //mqtt->sync_pub("0");
}