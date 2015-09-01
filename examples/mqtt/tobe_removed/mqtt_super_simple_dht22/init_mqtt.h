// MQTT INITIALIZER
void init_mqtt()
{

  mqtt = new MqttConnector(MQTT_HOST, MQTT_PORT);

  mqtt->prepare_configuration([&](MqttConnector::Config * config) -> void {
    config->enableLastWill = true;
    config->retainPublishMessage = false;

    #if defined (MQTT_USERNAME) && defined (MQTT_PASSWORD)
      config->username = String(MQTT_USERNAME);
      config->password = String(MQTT_PASSWORD);
    #endif

  });

  mqtt->after_prepare_configuration([&](MqttConnector::Config config) -> void {
    Serial.print(String("HOST: ") + config.mqttHost);
    Serial.println(String(" PORT: ") + config.mqttPort);
    Serial.println(String("__PUBLICATION TOPIC .. ") + config.topicPub);
    Serial.println(String("__SUBSCRIPTION TOPIC .. ") + config.topicPub);
  });

  mqtt->prepare_data(on_prepare_data, PUBLISH_EVERY);
  mqtt->prepare_subscribe([&](MQTT::Subscribe * sub) -> void { });
  mqtt->after_prepare_data([&](JsonObject * root) -> void {
    JsonObject& data = (*root)["d"];
    // data.remove("chip_id");
  });

  // on_message_arrived located in _receive.h
  mqtt->on_message(on_message_arrived);

  mqtt->on_connecting([&](int count, bool * flag) {
    Serial.print("MQTT CONNECTING.. ");
    Serial.println(count);
  });

  mqtt->on_published([&](const MQTT::Publish & pub) -> void {
    Serial.print("PUBLISHED: ");
    Serial.println(pub.payload_string());
  });

  mqtt->connect();
}