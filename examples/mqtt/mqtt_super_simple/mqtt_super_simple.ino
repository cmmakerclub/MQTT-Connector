#define MQTT_DEBUG_MODE
#define WIFI_DEBUG_MODE

#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <pt.h>

MqttConnector *mqtt;
WiFiConnector *wifi;

void init_hardware()
{
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println("BEGIN");
  Serial.println("BEGIN");
  Serial.println("BEGIN");
}

void init_wifi()
{
  // use flash memory ssid & smartconfig
  wifi = new WiFiConnector();

  // wifi = new WiFiConnector(ssid, password);

  wifi->on_connecting([&](const void* message)
  {
    Serial.println("connecting ");
    // Serial.println ((char*)message);
    delay(500);
  });

  wifi->on_connected([&](const void* message)
  {
    Serial.println("WIFI CONECTED: ");
    // Print the IP address
    Serial.println(WiFi.localIP());
    // Serial.println ((char*)message);
  });

  wifi->on_disconnected([&](const void* message)
  {
    Serial.println("WIFI DISCONECTED.");
    // Serial.println ((char*)message);
  });

  wifi->on_smartconfig_enter([&](const void* message)
  {
    Serial.println("ENTER SMARTCONFIG.");
  });

  wifi->on_smartconfig_done([&](const void* message)
  {
    Serial.println("SMARTCONFIG DONE.");
  });

  wifi->on_smartconfig_doing([&](const void* message)
  {
    // Serial.println("CONFIGURING WIFI..");
    // delay(500);
  });

  wifi->connect();

}
void init_mqtt()
{

  mqtt = new MqttConnector("iot.eclipse.org");
  mqtt->prepare_configuration([&](MqttConnector::Config * config) -> void {
    // do nothing.
    Serial.print("HOST: ");
    Serial.print(config->mqttHost);

    Serial.print(" PORT: ");
    Serial.println(config->mqttPort);

    Serial.print("__PUBLICATION TOPIC -> ");
    Serial.println(config->topicPub);
    Serial.print("__SUBSCRIPTION TOPIC -> ");
    Serial.println(config->topicSub);    
  });

  mqtt->prepare_data([&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    data["myName"] = "NAzT";
    data["adc"] = analogRead(A0);
    data["tag"] = "paris";
    data["zone"] = "1";
  }, 5000);


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

  mqtt->on_message([&](const MQTT::Publish & pub) -> void {
    Serial.print("ON MESSAGE: ");
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
  });

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