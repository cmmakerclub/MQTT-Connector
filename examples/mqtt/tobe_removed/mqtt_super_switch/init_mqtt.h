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
  });

  mqtt->prepare_data(on_prepare_data, 15 *1000);


  mqtt->prepare_subscribe([&](MQTT::Subscribe * sub) -> void { });


  mqtt->after_prepare_data([&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");

    // Serial.println("DATA: ");
    // data.printTo(Serial);
    // Serial.println();
    // Serial.println();
    // Serial.println();

    data.remove("version");
    data.remove("flash_id");
    data.remove("flash_size");
    data.remove("chip_id");
    data.remove("sdk");
    data.remove("counter");
  });

  mqtt->on_message(on_message_arrived);

  mqtt->on_connecting([&](int count, bool * flag) {
    Serial.print(count);
    Serial.println("MQTT CONNECTING..: ");
    if (count > 60000) {
      *flag = false;
    }
  });

  mqtt->on_published([&](const MQTT::Publish & pub) -> void {
    Serial.print("PUBLISHED: ");
    Serial.println(pub.payload_string());
  });

  mqtt->connect();
}