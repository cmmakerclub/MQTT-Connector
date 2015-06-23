#define DEBUG_MODE
#define DEBUG_LEVEL_VERBOSE

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "dht_helper.h"

const char* ssid     = "CMMC.47";
const char* pass     = "guestnetwork";

MqttWrapper *mqtt;
DHT *dht;

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


void hook_configuration(MqttWrapper::Config config) {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    String result;
    for (int i = 0; i < 6; ++i)
    {
        result += String(mac[i], 16);
    }

    *(config.clientId) = String("vega-") + result;
    *(config.channelId) = String("esp8266/vega/");
}

void hook_prepare_data(JsonObject** root) {
  JsonObject& data = (*(*root))["d"];

  static float t_dht;
  static float h_dht;
  read_dht(dht, &t_dht, &h_dht);

  data["myName"] = "vega-005";
  data["temp"] = t_dht;
  data["humid"] = h_dht;
}

void setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println();
    Serial.println();

    connect_wifi();
    init_dht(&dht, DHTPIN, DHTTYPE);

    mqtt = new MqttWrapper("128.199.104.122", 1883, hook_configuration);
    mqtt->connect();
    mqtt->set_prepare_data_hook(hook_prepare_data);
}

void loop() {
    reconnect_wifi_if_link_down();
    mqtt->loop();
}