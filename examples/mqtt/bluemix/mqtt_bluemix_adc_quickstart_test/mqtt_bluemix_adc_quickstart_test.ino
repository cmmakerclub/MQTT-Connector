// #define DEBUG_MODE
// #define DEBUG_LEVEL_VERBOSE

#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include "mqtt_configuration.h"

const char* ssid  = "NAT.WRTNODE";
const char* pass  = "devicenetwork";

MqttConnector *mqtt;
WiFiConnector *wifi;


void hook_prepare_data(JsonObject* root)
{
    JsonObject& data = root->at("d");

    data["myName"] = "NAT";
    data["adc"] = analogRead(A0);

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
    Serial.println("START..");
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
    String organization = String("quickstart");
    String url = organization + ".messaging.internetofthings.ibmcloud.com";

    mqtt = new MqttConnector(url.c_str()  , 1883, hook_configuration);
    mqtt->set_prepare_data_hook(hook_prepare_data, 2000);
    mqtt->set_after_prepare_data_hook([] (JsonObject *root) -> void {
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