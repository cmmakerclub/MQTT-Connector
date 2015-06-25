#define DEBUG_MODE

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <WiFiHelper.h>
#include <PubSubClient.h>

const char* ssid     = "CMMC.32";
const char* pass     = "guestnetwork";

MqttWrapper *mqtt;
WiFiHelper *wifi;


void callback(const MQTT::Publish& pub)
{
    if (pub.payload_string() == "0")
    {
        // digitalWrite(5, LOW);
        Serial.print(" => ");
        Serial.println(pub.payload_string());
    }
    else if(pub.payload_string() == "1")
    {
        // digitalWrite(5, HIGH);      
        Serial.print(" => ");
        Serial.println(pub.payload_string());
    }
    else
    {
        Serial.print(pub.topic());
        Serial.print(" => ");
        Serial.println(pub.payload_string());
    }
}

void init_wifi()
{
    wifi = new WiFiHelper(ssid, pass);
    wifi->begin();
}

void init_mqtt()
{
    mqtt = new MqttWrapper("iot.eclipse.org");
    mqtt->connect(callback);
    mqtt->set_prepare_data_hook([](JsonObject** root)
    {
        JsonObject& data = (*root)->at("d");
        data["myName"] = "SIMPLE-TEST";
    });
}

void init_hardware()
{
    Serial.begin(115200);
    pinMode(0, INPUT_PULLUP);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    delay(10);
    Serial.println();
    Serial.println();
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

    // ตรวจจับการกด Switch
    if (digitalRead(0) == LOW)
    {
        // วนลูปจนกว่าจะเอาปล่อย Switch
        while(digitalRead(0) == LOW)
        {
            mqtt->loop();
            yield();
        }
        // ปล่่อยปลุ่มค่อยส่งค่า
        String status = "0";
        mqtt->sync_pub(status);
    }
}