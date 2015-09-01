PubSubClient::callback_t on_message_arrived = 
[&](const MQTT::Publish & pub) -> void {
    Serial.print("ON MESSAGE: ");
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
    celsius = pub.payload_string();
 };