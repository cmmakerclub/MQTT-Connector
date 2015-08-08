#define MQTT_DEBUG_MODE
#define WIFI_DEBUG_MODE

#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <pt.h>


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

  wifi->on_connecting([](const void* message)
  {
    Serial.print("connecting ");
    Serial.println ((char*)message);
  });

  wifi->on_connected([](const void* message)
  {
    Serial.print ("WIFI CONECTED: ");
    Serial.println ((char*)message);
  });

  wifi->on_disconnected([](const void* message)
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
  mqtt = new MqttConnector("cmmc.xyz");
  mqtt->set_prepare_data_hook([] (JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    data["myName"] = "SIMPLE_WILL";
  }, 15000);

  mqtt->set_after_prepare_data_hook([](JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    //delete data["version"];
  });
 
  mqtt->on_message([](const MQTT::Publish &pub) -> void {
    Serial.print("ON MESSAGE: ");
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
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