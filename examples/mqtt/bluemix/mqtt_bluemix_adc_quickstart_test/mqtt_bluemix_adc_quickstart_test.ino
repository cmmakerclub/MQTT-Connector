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

  String organization = String("quickstart");
  String url = organization + ".messaging.internetofthings.ibmcloud.com";
  mqtt = new MqttConnector(url.c_str());
  // mqtt = new MqttConnector("cmmc.xyz");
  // mqtt = new MqttConnector("mqtt.tespa.io");



  mqtt->prepare_configuration([&](MqttConnector::Config * config) -> void {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    String macAddress;
    for (int i = 0; i < 6; ++i)
    {
      macAddress += String(mac[i], 16);
    }


    config->clientId  = String("d:quickstart:cmmakerclub.com:");
    config->clientId += macAddress;
    config->topicPub  = "iot-2/evt/status/fmt/json";
    String url  = String("https://quickstart.internetofthings.");
    url += "ibmcloud.com/#/device/" + macAddress + "/sensor/";

    Serial.println(url);

  });

  // mqtt->prepare_configuration(NULL);

  mqtt->prepare_data([&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    data["adc"] = analogRead(A0);
  }, 1500);


  mqtt->prepare_subscribe(NULL);

  // mqtt->prepare_subscribe([&](MQTT::Subscribe *sub) -> void {
  //   // (sub)->add_topic("HELLO");
  // });

  mqtt->after_prepare_data([&](JsonObject *root) -> void {
    Serial.println("AFTER PREPARE");
    JsonObject& data = root->at("d");
    //data.remove("flash_size");
    //data.remove("flash_id");
    //data.remove("version");
  });

  mqtt->on_message([&](const MQTT::Publish & pub) -> void {
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