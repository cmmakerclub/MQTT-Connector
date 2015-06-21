#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <PubSubClient.h>

const char* ssid     = "ClickExec-IAP";
const char* pass = "click2499";

#define WIFI_MAX_RETRIES 1500
#define WIFI_CONNECT_DELAY_MS 20



MqttWrapper *mqtt;

static StaticJsonBuffer<800> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

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

void hook_before_publish(char** dataPtr) {
  Serial.println("INO BEFORE PUB");
  String s = "HELLO WORLD";
  *dataPtr = "HELLO";

  static char payload[800];
  static long counter = 0;
  root["counter"] = ++counter;

  root.printTo(payload, sizeof(payload));  

  *dataPtr = payload;
}

void hook_config(MqttWrapper::Config config) {
  Serial.println("+++++++++++++++++++");
  Serial.println("IN HOOK CONFIG INO");
  Serial.print("CLIENT ID: ");
  Serial.println(*(config.clientId));

  Serial.println(config.client->server_hostname);
  Serial.println("-------------------");
}



void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("GO");

  connectWifi();

  mqtt = new MqttWrapper("128.199.104.122", 1883, hook_config);
  mqtt->connect(callback);
  mqtt->set_before_publish_hook(hook_before_publish);
}

void loop() {
  mqtt->loop();
}