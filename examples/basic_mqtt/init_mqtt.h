#include <MqttConnector.h>

// MQTT INITIALIZER
void init_mqtt()
{
  mqtt = new MqttConnector(MQTT_HOST, MQTT_PORT);
  mqtt->on_prepare_configuration([&](MqttConnector::Config * config) -> void {
  config->clientId  = String(MQTT_CLIENT_ID);
  config->channelPrefix = String(MQTT_PREFIX);
  config->enableLastWill = true;
  config->retainPublishMessage = true;
  config->publishOnly = false;
  config->subscribeOnly = false;
  config->firstCapChannel = false;

  config->username = String(MQTT_USERNAME);
  config->password = String(MQTT_PASSWORD);

  uint8_t mac[6];
  WiFi.macAddress(mac);

  // device id
  String macAddr;
  for (int i = 0; i < 6; ++i)
  {
      macAddr += String(mac[i], 16);
  }

    // FORMAT
    // d:quickstart:<type-id>:<device-id>
    // config->clientId += macAddr;
    //config->topicPub  = String("/HelloChiangMaiMakerClub/gearname/") + config->clientId;
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
    Serial.print("[MQTT] PUBLISHED: ");
    Serial.println(pub.payload_string());
  });

  mqtt->connect();
}
