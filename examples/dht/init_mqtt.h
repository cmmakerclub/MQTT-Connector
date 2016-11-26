#include <MqttConnector.h>

extern MqttConnector::prepare_data_hook_t on_prepare_data;
extern MqttConnector* mqtt;
extern PubSubClient::callback_t on_message_arrived;
extern CMMC_Blink blinker;

extern const char *MQTT_HOST;
extern const char *MQTT_USERNAME;
extern const char *MQTT_PASSWORD;
extern const char *MQTT_CLIENT_ID;
extern const char *MQTT_PREFIX;

extern const int MQTT_PORT;
extern const int PUBLISH_EVERY;

// MQTT INITIALIZER
void init_mqtt()
{
  mqtt = new MqttConnector(MQTT_HOST, MQTT_PORT);
  mqtt->on_prepare_configuration([&](MqttConnector::Config * config) -> void {
  config->clientId  = String(MQTT_CLIENT_ID);
  config->channelPrefix = String(MQTT_PREFIX);
  config->enableLastWill = true;
  config->retainPublishMessage = false;
  /*
   *  config->mode
   *  ===================
   *  | MODE_BOTH       |
   *  | MODE_PUB_ONLY   |
   *  | MODE_SUB_ONLY   |
   *  ===================
  */
  config->mode = MODE_BOTH;
  config->firstCapChannel = false;

  config->username = String(MQTT_USERNAME);
  config->password = String(MQTT_PASSWORD);

  // FORMAT
  // d:quickstart:<type-id>:<device-id>
  //config->clientId  = String("d:quickstart:esp8266meetup:") + macAddr;
  //config->topicPub  = String("iot-2/evt/status/fmt/json");

  });

  mqtt->on_after_prepare_configuration([&](MqttConnector::Config config) -> void {
    Serial.printf("[USER] HOST = %s\r\n", config.mqttHost.c_str());
    Serial.printf("[USER] PORT = %s\r\n", String(config.mqttHost).c_str());
    Serial.printf("[USER] PUB  = %s\r\n", config.topicPub.c_str());
    Serial.printf("[USER] SUB  = %s\r\n", config.topicSub.c_str());
  });

  mqtt->on_prepare_data(on_prepare_data, PUBLISH_EVERY);
  mqtt->on_prepare_subscribe([&](MQTT::Subscribe * sub) -> void { });
  mqtt->on_after_prepare_data([&](JsonObject * root) -> void {
    /**************
    remove prepared data from lib
    root->remove("info");
    JsonObject& data = (*root)["d"];
    data.remove("version");
    data.remove("subscription");
    **************/
  });

  // on_message_arrived located in _receive.h
  mqtt->on_message(on_message_arrived);

  mqtt->on_connecting([&](int count, bool * flag) {
    Serial.printf("[%lu] MQTT CONNECTING.. \r\n", count);
    delay(1000);
  });

  mqtt->on_published([&](const MQTT::Publish & pub) -> void {
    static int counter = 0;
    Serial.print("[MQTT] PUBLISHED: COUNTER = ");
    Serial.println(counter++);
    if (counter == 10) {
      // blinker.blink(100, LED_BUILTIN);
      // delay(1000);
      // ESP.deepSleep(45 * 1000000);
    }
    else {
      digitalWrite(LED_BUILTIN, LOW);
      delay(10);
      digitalWrite(LED_BUILTIN, HIGH);
    }
  });

  mqtt->connect();
}
