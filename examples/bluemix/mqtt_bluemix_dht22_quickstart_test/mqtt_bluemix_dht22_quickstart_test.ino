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
#include "mqtt_configuration.h"

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

void hook_publish_data(char* data)
{
    Serial.print("PUBLISH: ->");
    Serial.println(data);
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

void init_wifi()
{
    wifi = new WiFiHelper(ssid, pass);

    wifi->on_connecting([](const char* message)
    {
        Serial.println("connecting...");  
    });
    
    wifi->on_connected([](const char* message)
    {
        Serial.println("");
        Serial.println("WiFi connected");  
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    });

    wifi->connect();
}

void init_mqtt()
{
    mqtt = new MqttWrapper("quickstart.messaging.internetofthings.ibmcloud.com", 1883, hook_configuration);
    mqtt->set_prepare_data_hook(hook_prepare_data, 2000);
    mqtt->set_publish_data_hook(hook_publish_data);
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
    wifi->loop();
    mqtt->loop();
}