#define MQTT_DEBUG_MODE
#define WIFI_DEBUG_MODE

#include <DHT.h>
#include "dht_helper.h"
#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif


MqttConnector *mqtt;
WiFiConnector *wifi;
DHT *dht;


void init_hardware()
{
  Serial.begin(9600);
  delay(10);
  Serial.println();

  #define DHTPIN 12    // what pin we're connected to
  #define DHTTYPE DHT22   // DHT 22  (AM2302)
 
  pinMode(DHTPIN, INPUT_PULLUP);
  pinMode(14, OUTPUT);    
  pinMode(13, OUTPUT);    
  pinMode(16, OUTPUT);      
  digitalWrite(13, HIGH);
  digitalWrite(14, LOW);    
  digitalWrite(16, LOW);      

  init_dht(&dht, DHTPIN, DHTTYPE);

  Serial.println("BEGIN");
  Serial.println("BEGIN");
  Serial.println("BEGIN");
}

void init_wifi()
{
  // use flash memory ssid & smartconfig
   wifi = new WiFiConnector("NAT.WRTNODE", "devicenetwork");
   //wifi = new WiFiConnector();


  wifi->on_connecting([&](const void* message)
  {
    Serial.print("connecting -> ");
    Serial.println(wifi->counter);
    Serial.println(wifi->get("ssid") + ", " + wifi->get("password"));
    //if (wifi->counter > 100) {
      //wifi->enter_smartconfig_mode();
    //}
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

  mqtt = new MqttConnector("cmmc.xyz");

  mqtt->on_message([&](const MQTT::Publish & pub) -> void {
    Serial.print("ON MESSAGE: ");
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
  });

  mqtt->prepare_configuration([&](MqttConnector::Config * config) -> void {
    Serial.println("=========================");
    Serial.print("HOST: ");
    Serial.print(config->mqttHost);

    Serial.print(" PORT: ");
    Serial.println(config->mqttPort);
    Serial.println("=========================");

    
    Serial.print("__PUBLICATION TOPIC -> ");
    Serial.println(config->topicPub);
    Serial.print("__SUBSCRIPTION TOPIC -> ");
    Serial.println(config->topicSub);    


    Serial.print(" ENABLE WILL STATUS: ");
    config->enableLastWill = true;
    Serial.println(config->enableLastWill);
    Serial.println("=========================");

  });

  mqtt->prepare_data([&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    float t_dht;
    float h_dht;

    data["myName"] = "DEVKIT-DHT22";
    data["tag"] = "Nat";
    data["board"] = "espminiv2";
    data["sensor"] = "DHT22";

    read_dht(dht, &t_dht, &h_dht);
   
    data["temp"] = t_dht;
    data["humid"] = h_dht;
   

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

//    data.remove("version");
//    data.remove("flash_id");
//    data.remove("flash_size");
//    data.remove("chip_id");
//    data.remove("sdk");
//    data.remove("counter");
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