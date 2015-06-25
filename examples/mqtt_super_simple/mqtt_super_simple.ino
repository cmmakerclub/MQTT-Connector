#define DEBUG_MODE

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <WiFiHelper.h>
#include <PubSubClient.h>

const char* ssid     = "CMMC.47";
const char* pass     = "guestnetwork";

MqttWrapper *mqtt;
WiFiHelper *wifi;


void on_message_arrived(const MQTT::Publish& pub)
{
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
}

void hook_prepare_data(JsonObject** root) {
   JsonObject& data = (*root)->at("d");
   data["myName"] = "SIMPLE-TEST"; 
}

void init_hardware()
{
    Serial.begin(115200);
    delay(10);
    Serial.println();
    Serial.println();
}

void init_wifi()
{
    wifi = new WiFiHelper(ssid, pass);
    wifi->connect();
}

void init_mqtt()
{
    mqtt = new MqttWrapper("iot.eclipse.org");
    mqtt->set_prepare_data_hook(hook_prepare_data);
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
    wifi->loop();
    mqtt->loop(); 
}