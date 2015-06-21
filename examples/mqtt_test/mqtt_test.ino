#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <PubSubClient.h>

const char* ssid     = "ClickExec-IAP";
const char* pass = "click2499";
// const char* ssid     = "OpenWrt_NAT_500GP.101";
// const char* pass = "activegateway";

#define WIFI_MAX_RETRIES 1500
#define WIFI_CONNECT_DELAY_MS 20



MqttWrapper *mqtt;


void callback(const MQTT::Publish& pub) {
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
}

void connectWifi()
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

void hook_before_publish(JsonObject** root) {
  JsonObject& data = (*(*root))["d"];

  data["myName"] = "NAT";
}

void hook_publish_data(char** dataPtr) {
}

void hook_before_connect(MqttWrapper::Config config) {
    Serial.println("+++++++++++++++++++");
    Serial.println("IN HOOK CONFIG INO");
    Serial.print("CLIENT ID: ");
    Serial.println(*(config.clientId));
    Serial.println(*(config.topic_sub));

    // *(config.clientId) = "d:quickstart:arduino:18fe34a07e58";
    // *(config.topic_pub) = "iot-2/evt/status/fmt/json";

    Serial.println("-------------------");
}



void setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println("GO");

    connectWifi();

    mqtt = new MqttWrapper("128.199.104.122", 1883, hook_before_connect);
    // mqtt = new MqttWrapper("quickstart.messaging.internetofthings.ibmcloud.com", 1883, hook_before_connect);
    mqtt->connect(callback);
    mqtt->set_before_publish_hook(hook_before_publish);
}

void loop() {
    mqtt->loop();
}