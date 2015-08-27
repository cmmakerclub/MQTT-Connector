// MQTT INITIALIZER
void init_mqtt()
{

  mqtt = new MqttConnector(MQTT_HOST, MQTT_PORT);

  mqtt->prepare_configuration([&](MqttConnector::Config * config) -> void {
    Serial.print("HOST: ");
    Serial.print(config->mqttHost);

    Serial.print(" PORT: ");
    Serial.println(config->mqttPort);

    Serial.print("__PUBLICATION TOPIC -> ");
    Serial.println(config->topicPub);
    Serial.print("__SUBSCRIPTION TOPIC -> ");
    Serial.println(config->topicSub);

    config->username = String("test");
    config->password = String("test");    
    
  });

  mqtt->prepare_data(on_prepare_data, PUBLISH_EVERY);


  mqtt->prepare_subscribe([&](MQTT::Subscribe * sub) -> void { });


  mqtt->after_prepare_data([&](JsonObject * root) -> void {
    JsonObject& data = (*root)["d"];
    // data.remove("chip_id");
  });

  mqtt->on_message(on_message_arrived);

  mqtt->on_connecting([&](int count, bool * flag) {
    Serial.print(count);
    Serial.println("MQTT CONNECTING..: ");
  });

  mqtt->on_published([&](const MQTT::Publish & pub) -> void {
    Serial.print("PUBLISHED: ");
    Serial.println(pub.payload_string());
  });

  mqtt->connect();
}