//#define DEBUG_MODE
// #define DEBUG_LEVEL_VERBOSE

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <WiFiHelper.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "dht_helper.h"

const char* ssid     = "CMMC.47";
const char* pass     = "guestnetwork";

MqttWrapper *mqtt;
WiFiHelper *wifi;
DHT *dht;

void hook_prepare_data(JsonObject** root)
{
    JsonObject& data = (*(*root))["d"];

    static float t_dht;
    static float h_dht;

    read_dht(dht, &t_dht, &h_dht);

    data["myName"] = "NAT-DHT22-TEST";
    data["temp"] = t_dht;
    data["humid"] = h_dht;

}

void hook_configuration(MqttWrapper::Config config)
{
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

void hook_publish_data(char* data)
{
    Serial.print("PUBLISH: ->");
    Serial.println(data);
}


void init_wifi()
{
    wifi = new WiFiHelper(ssid, pass);
    wifi->on_connected([](const char* message)
    {
        Serial.println (message);
    });
    wifi->on_disconnected([](const char* message)
    {
        Serial.println (message);
    });
    wifi->begin();
}

void init_mqtt()
{
    mqtt = new MqttWrapper("quickstart.messaging.internetofthings.ibmcloud.com", 1883, hook_configuration);
    mqtt->connect();
    mqtt->set_prepare_data_hook(hook_prepare_data, 2000);
    mqtt->set_publish_data_hook(hook_publish_data);
}


void init_hardware()
{
    Serial.begin(115200);
    pinMode(0, INPUT_PULLUP);
    delay(10);
    Serial.println();
    Serial.println();
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
    wifi->loop();
    mqtt->loop();
}