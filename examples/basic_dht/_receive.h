extern String MQTT_CLIENT_ID;

MqttConnector::after_message_arrived_t on_after_message_arrived =
[&](String topic, String cmd, String payload) -> void {
  Serial.printf("topic: %s\r\n", topic.c_str());
  Serial.printf("cmd: %s\r\n", cmd.c_str());
  Serial.printf("payload: %s\r\n", payload.c_str());
  if (cmd == "$/reboot") {
    ESP.reset();
  }
};

MqttConnector::subscribe_hook_t on_subscribe =
[&](MQTT::Subscribe *sub) -> void {
  sub->add_topic(MQTT_PREFIX + "/" + DEVICE_NAME + "/$/+");
  sub->add_topic(MQTT_PREFIX + "/" + MQTT_CLIENT_ID + "/$/+");
};

PubSubClient::callback_t on_message_arrived = NULL;
