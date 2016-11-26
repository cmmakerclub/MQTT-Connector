PubSubClient::callback_t on_message_arrived =
[&](const MQTT::Publish &pub) -> void {
    String topic = pub.topic();
    String payload = pub.payload_string();
    Serial.printf("TOPIC = %s, PAYLOAD = %s\r\n", topic.c_str(), payload.c_str());
 };

MqttConnector::subscribe_hook_t on_subscribe = [&](MQTT::Subscribe *sub) {

};
