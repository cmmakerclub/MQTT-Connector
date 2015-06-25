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

void callback(const MQTT::Publish& pub)
{
    if (pub.payload_string() == "0")
    {
        Serial.print(" => ");
        Serial.println(pub.payload_string());
    }
    else if (pub.payload_string() == "1")
    {
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

void hook_prepare_data(JsonObject** root)
{
    JsonObject& data = (*(*root))["d"];

    data["myName"] = "NAT";
    data["adc"] = analogRead(A0);;

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

    *(config.clientId) = String("d:quickstart:arduino:") + result;
    *(config.username) = String("test");
    *(config.password) = String("test");
    *(config.channelId) = String("esp8266/");
    *(config.topicPub) = "iot-2/evt/status/fmt/json";
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
    mqtt = new MqttWrapper("m20.cloudmqtt.com", 19642, hook_configuration);
    mqtt->connect(callback);
    mqtt->set_prepare_data_hook(hook_prepare_data, 5000);
    mqtt->set_publish_data_hook(hook_publish_data);
}

void init_hardware()
{
    Serial.begin(115200);
    pinMode(0, INPUT_PULLUP);
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
        while (digitalRead(0) == LOW)
        {
            mqtt->loop();
            yield();
        }
        String status = "0";
        mqtt->sync_pub(status);
    }

}