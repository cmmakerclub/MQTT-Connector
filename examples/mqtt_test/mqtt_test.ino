#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <PubSubClient.h>

const char* ssid     = "ClickExec-IAP";
const char* pass = "click2499";

#define WIFI_MAX_RETRIES 1500
#define WIFI_CONNECT_DELAY_MS 20



MqttWrapper *mqtt;


void callback(const MQTT::Publish& pub) { 
  Serial.println("CALLBACK FRoM INO");
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

    Serial.println(" WIFI CONNECTED ");
}



void setup() {

  Serial.begin(115200);
  delay(10);
  Serial.println("GO");

  connectWifi();

  mqtt = new MqttWrapper("128.199.104.122");

  mqtt->begin();
  mqtt->set_callback(callback);

  mqtt->connect();
  
}

void loop() {
  mqtt->loop();
}