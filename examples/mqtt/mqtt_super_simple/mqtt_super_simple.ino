#define MQTT_DEBUG_MODE
#define WIFI_DEBUG_MODE

#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

MqttConnector *mqtt;
WiFiConnector *wifi;

// MQTT_HOSTT
#include "init_wifi.h"
#include "_publish.h"
#include "_receive.h"

#define MQTT_HOST "cmmc.xyz"
#define MQTT_PORT 1883

void init_hardware()
{
  Serial.begin(9600);
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



// MQTT INITIALIZER
void init_mqtt()
{

  mqtt = new MqttConnector(MQTT_HOST, MQTT_PORT);

  mqtt->prepare_configuration([&](MqttConnector::Config * config) -> void {
    Serial.print("HOST: ");
    Serial.print(config->mqttHost);

    Serial.print(" PORT: ");
    Serial.println(config->mqttPort);

    Serial.print("__PUBLICATION TOPIC -> ");
    Serial.println(config->topicPub);
    Serial.print("__SUBSCRIPTION TOPIC -> ");
    Serial.println(config->topicSub);    
  });

  mqtt->prepare_data(on_prepare_data, 5000);


  mqtt->prepare_subscribe([&](MQTT::Subscribe * sub) -> void {
  });


  mqtt->after_prepare_data([&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");

    // Serial.println("DATA: ");
    // data.printTo(Serial);
    // Serial.println();
    // Serial.println();
    // Serial.println();

    data.remove("version");
    data.remove("flash_id");
    data.remove("flash_size");
    data.remove("chip_id");
    data.remove("sdk");
    data.remove("counter");
  });

  mqtt->on_message(on_message_arrived);

  mqtt->on_connecting([&](int count, bool * flag) {
    Serial.print("MQTT CONNECTING..: ");
    Serial.println(count);
  });

  mqtt->on_published([&](const MQTT::Publish & pub) -> void {
    Serial.print("PUBLISHED: ");
    Serial.println(pub.payload_string());
  });

  mqtt->connect();
}