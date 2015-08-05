
#define MQTT_DEBUG_MODE 1


#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <MqttConnector.h>
#include <WiFiConnector.h>

const char* ssid     = "NAT.11";
const char* pass     = "guestnetwork";

MqttConnector *mqtt;
WiFiConnector *wifi;
  
void on_message_arrived(const MQTT::Publish& pub)
{
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
}

void hook_prepare_data(JsonObject* root) {
   JsonObject& data = root->at("d");
   data["myName"] = "OD-WIFI-XYZ-TEST";
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
    wifi->connect();
}

void init_mqtt()
{
    mqtt = new MqttConnector("cmmc.xyz");
    mqtt->set_prepare_data_hook(hook_prepare_data, 500);
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
    mqtt->sync_pub("0");
}
