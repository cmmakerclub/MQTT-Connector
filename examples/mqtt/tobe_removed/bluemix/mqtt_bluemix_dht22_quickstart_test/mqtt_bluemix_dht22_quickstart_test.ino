//#define DEBUG_MODE
// #define DEBUG_LEVEL_VERBOSE

// #define DEBUG_MODE
// #define DEBUG_LEVEL_VERBOSE

#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <DHT.h>
#include "dht_helper.h"
#include "mqtt_configuration.h"


const char* ssid  = "NAT.WRTNODE";
const char* pass  = "devicenetwork";


MqttConnector *mqtt;
WiFiConnector *wifi;
DHT *dht;

void hook_prepare_data(JsonObject* root)
{
    JsonObject& data = root->at("d");

    static float t_dht;
    static float h_dht;

    read_dht(dht, &t_dht, &h_dht);

    data["myName"] = "WETH-002";
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
    delay(10);
    Serial.println("OK OK");
    Serial.println("OK OK");
    Serial.println("OK OK");
    Serial.println();
    init_dht(&dht, DHTPIN, DHTTYPE);
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
    //String organization = String("quickstart");
    //String url = organization + ".messaging.internetofthings.ibmcloud.com";

    mqtt = new MqttConnector("cmmc.xyz");
    mqtt->prepare_data(hook_prepare_data, 2000);
    mqtt->after_prepare_data([] (JsonObject *root) -> void {
        JsonObject& data = root->at("d");
        //delete data["version"];
    });

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