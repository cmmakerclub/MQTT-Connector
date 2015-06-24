#define DEBUG_MODE
#define DEBUG_LEVEL_VERBOSE

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <PubSubClient.h>
#include <WiFiHelper.h>


const char* ssid     = "CMMC.32";
const char* pass     = "guestnetwork";

MqttWrapper *mqtt;
WiFiHelper *wifi;

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

    *(config.clientId) = String("a:r6crrd:") + result;
    Serial.println(*(config.clientId));

    // uncomment when user & password is required.
    *(config.username) = String("a-r6crrd-3gsgqcadid");
    *(config.password) = String("KRE*0rxt0rilV!2tD@");
    // *(config.channelId) = String("esp8266/");

    *(config.topicPub) = String("iot-2/type/") + String("esp8266/id/") + result + String("/evt/dw.mini/fmt/json");
    Serial.println(
        String("https://quickstart.internetofthings.") +
        "ibmcloud.com/#/device/"+
        result +"/sensor/"
    );

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
    mqtt = new MqttWrapper("r6crrd.messaging.internetofthings.ibmcloud.com", 1883, hook_configuration);
    mqtt->connect();
    mqtt->set_prepare_data_hook(hook_prepare_data, 2000);
    mqtt->set_publish_data_hook(hook_publish_data);
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