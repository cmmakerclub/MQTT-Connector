//#define DEBUG_MODE
// #define DEBUG_LEVEL_VERBOSE 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "dht_helper.h"

// const char* ssid     = "OpenWrt_NAT_500GP.101";
// const char* pass = "activegateway";

// const char* ssid     = "MAKERCLUB-CM";
// const char* pass = "welcomegogogo";

const char* ssid     = "Opendream Play";
const char* pass = "5k,skrijv',7'sik";

// const char* ssid     = "Opendream";
// const char* pass = "gfkgvkgv'2015!!!!";


MqttWrapper *mqtt;
DHT *dht;

void connect_wifi()
{
    WiFi.begin(ssid, pass);

    int retries = 0;
    while ((WiFi.status() != WL_CONNECTED))
    {
        Serial.print(".");
        retries++;
        delay(500);
    }

    Serial.println("WIFI CONNECTED ");
}

void reconnect_wifi_if_link_down() {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("WIFI DISCONNECTED");
        connect_wifi();
    }
}

void hook_prepare_data(JsonObject** root) {
  JsonObject& data = (*(*root))["d"];

  static float t_dht;
  static float h_dht;
  read_dht(dht, &t_dht, &h_dht);



  data["myName"] = "SIMPLE-DHT22-TEST";
  data["temp"] = t_dht;
  data["humid"] = h_dht;

}

void hook_configuration(MqttWrapper::Config config) {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    String result;
    for (int i = 0; i < 6; ++i)
    {
        result += String(mac[i], 16);
    }

    *(config.clientId) = String("d:quickstart:esp8266:") + result;
    *(config.topicPub) = "iot-2/evt/status/fmt/json";
    Serial.println(
        String("https://quickstart.internetofthings.") +
                "ibmcloud.com/#/device/"+
                result +"/sensor/"
              );
 
}

void hook_publish_data(char* data) {
    Serial.print("PUBLISH: ->");
    Serial.println(data);
}

void setup() {
    Serial.begin(115200);
    pinMode(0, INPUT_PULLUP);
    delay(10);
    Serial.println();
    Serial.println();

    init_dht(&dht, DHTPIN, DHTTYPE);    
    connect_wifi();

    mqtt = new MqttWrapper("quickstart.messaging.internetofthings.ibmcloud.com", 1883, hook_configuration);
    mqtt->connect();
    mqtt->set_prepare_data_hook(hook_prepare_data, 2000);
    mqtt->set_publish_data_hook(hook_publish_data);
}

void loop() {
    reconnect_wifi_if_link_down();
    mqtt->loop();
}