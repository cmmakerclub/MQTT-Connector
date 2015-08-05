#define MQTT_DEBUG_MODE
#define WIFI_DEBUG_MODE

#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>


const char* ssid     = "NAT.WRTNODE";
const char* pass     = "devicenetwork";

MqttConnector *mqtt;
WiFiConnector *wifi;
  
void on_message_arrived(const MQTT::Publish& pub)
{
    Serial.print("ON MESSAGE: ");
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
}

void hook_prepare_data(JsonObject* root) {
   JsonObject& data = root->at("d");
   data["myName"] = "SIMPLE_WILL";
}

void hook_after_prepare_data(JsonObject* root)
{
    JsonObject& data = root->at("d");
    //delete data["version"];
}

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
    mqtt->set_prepare_data_hook(hook_prepare_data, 5000);
    mqtt->set_after_prepare_data_hook(hook_after_prepare_data);
    mqtt->connect(on_message_arrived);
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
