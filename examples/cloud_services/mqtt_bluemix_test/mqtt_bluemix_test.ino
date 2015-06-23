#define DEBUG_MODE
// #define DEBUG_LEVEL_VERBOSE 

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <PubSubClient.h>

// const char* ssid     = "OpenWrt_NAT_500GP.101";
// const char* pass = "activegateway";

// const char* ssid     = "MAKERCLUB-CM";
// const char* pass = "welcomegogogo";

const char* ssid     = "Opendream Play";
const char* pass = "5k,skrijv',7'sik";

// const char* ssid     = "Opendream";
// const char* pass = "gfkgvkgv'2015!!!!";


MqttWrapper *mqtt;

void connect_wifi()
{
    WiFi.begin(ssid, pass);

    int retries = 0;
    while ((WiFi.status() != WL_CONNECTED))
    {
        Serial.print(".");
        retries++;
        delay(500);
    }

    Serial.println("WIFI CONNECTED ");
}

void reconnect_wifi_if_link_down() {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("WIFI DISCONNECTED");
        connect_wifi();
    }
}

void hook_prepare_data(JsonObject** root) {
  JsonObject& data = (*(*root))["d"];

  data["myName"] = "NAT";
  data["adc"] = analogRead(A0);;

}

void hook_configuration(MqttWrapper::Config config) {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    String result;
    for (int i = 0; i < 6; ++i)
    {
        result += String(mac[i], 16);
    }

    *(config.clientId) = String("d:quickstart:esp8266:") + result;

    // uncomment when user & password is required.
    // *(config.username) = String("test");
    // *(config.password) = String("test");
    // *(config.channelId) = String("esp8266/");

    *(config.topicPub) = "iot-2/evt/status/fmt/json";
    Serial.println(
        String("https://quickstart.internetofthings.") +
                "ibmcloud.com/#/device/"+
                result +"/sensor/"
              );
 
}

void hook_publish_data(char* data) {
    Serial.print("PUBLISH: ->");
    Serial.println(data);
}

void setup() {
    Serial.begin(115200);
    pinMode(0, INPUT_PULLUP);
    delay(10);
    Serial.println();
    Serial.println();

    connect_wifi();

    mqtt = new MqttWrapper("quickstart.messaging.internetofthings.ibmcloud.com", 1883, hook_configuration);
    mqtt->connect();
    mqtt->set_prepare_data_hook(hook_prepare_data, 2000);
    mqtt->set_publish_data_hook(hook_publish_data);
}

void loop() {
    reconnect_wifi_if_link_down();
    mqtt->loop();
}
