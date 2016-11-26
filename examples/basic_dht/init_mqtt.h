#include <MqttConnector.h>

extern MqttConnector* mqtt;
extern MqttConnector::prepare_data_hook_t on_prepare_data;
extern PubSubClient::callback_t on_message_arrived;
extern MqttConnector::after_prepare_data_hook_t on_after_prepare_data;
extern MqttConnector::before_prepare_data_once_t on_prepare_data_once;
extern MqttConnector::before_prepare_data_hook_t on_before_prepare_data_loop;
extern MqttConnector::subscribe_hook_t on_subscribe;

extern String MQTT_HOST;
extern String MQTT_USERNAME;
extern String MQTT_PASSWORD;
extern String MQTT_CLIENT_ID;
extern String MQTT_PREFIX;

extern int MQTT_PORT;
extern int PUBLISH_EVERY;

// MQTT INITIALIZER
void init_mqtt()
{
  mqtt = new MqttConnector(MQTT_HOST.c_str(), MQTT_PORT);

  mqtt->on_connecting([&](int count, bool *flag) {
    Serial.printf("[%lu] MQTT CONNECTING.. \r\n", count);
    delay(1000);
  });

  mqtt->on_prepare_configuration([&](MqttConnector::Config *config) -> void {
  config->clientId  = MQTT_CLIENT_ID;
  config->channelPrefix = MQTT_PREFIX;
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
    Serial.printf("[USER] PORT = %d\r\n", config.mqttPort);
    Serial.printf("[USER] PUB  = %s\r\n", config.topicPub.c_str());
    Serial.printf("[USER] SUB  = %s\r\n", config.topicSub.c_str());
  });

  mqtt->on_prepare_data(on_prepare_data, PUBLISH_EVERY);
  mqtt->on_subscribe(on_subscribe);
  mqtt->on_prepare_data_once(on_prepare_data_once);
  mqtt->on_before_prepare_data(on_before_prepare_data_loop);
  mqtt->on_after_prepare_data(on_after_prepare_data);
  mqtt->on_message(on_message_arrived);

  mqtt->connect();
}
