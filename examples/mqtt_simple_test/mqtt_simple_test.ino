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


void callback(const MQTT::Publish& pub) {
    if (pub.payload_string() == "0") {
        Serial.print(" => ");
        Serial.println(pub.payload_string());
    }
    else if(pub.payload_string() == "1") {
        Serial.print(" => ");
        Serial.println(pub.payload_string());
    }
    else {
        Serial.print(pub.topic());
        Serial.print(" => ");
        Serial.println(pub.payload_string());
    }
}

void hook_prepare_data(JsonObject** root) {
  JsonObject& data = (*(*root))["d"];

  data["myName"] = "SIMPLE-TEST";
  data["adc"] = analogRead(A0);;

}

void init_wifi() {
  wifi = new WiFiHelper(ssid, pass);
  wifi->on_connected([](const char* message) {    Serial.println (message); });
  wifi->on_disconnected([](const char* message) { Serial.println (message); });
  wifi->begin();
}

void init_mqtt() {
    mqtt = new MqttWrapper("128.199.104.122");
    mqtt->connect(callback);
    mqtt->set_prepare_data_hook(hook_prepare_data);
}

void setup() {
    Serial.begin(115200);
    pinMode(0, INPUT_PULLUP);
    delay(10);
    Serial.println();
    Serial.println();

    init_wifi();
    init_mqtt();

}

void loop() {
    wifi->loop();
    mqtt->loop();

    // ตรวจจับการกด Switch
    if (digitalRead(0) == LOW) {
        // วนลูปจนกว่าจะเอาปล่อย Switch
        while(digitalRead(0) == LOW) { 
            mqtt->loop();
            yield(); 
        }
        String status = "0";
        mqtt->sync_pub(status);
    }
}