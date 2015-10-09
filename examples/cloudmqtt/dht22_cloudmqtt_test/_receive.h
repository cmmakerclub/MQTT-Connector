PubSubClient::callback_t on_message_arrived = 
[&](const MQTT::Publish & pub) -> void {
    String topic = pub.topic();
    String payload = pub.payload_string();
    String text = topic + " => " + payload;

    Serial.println(text);
 };
