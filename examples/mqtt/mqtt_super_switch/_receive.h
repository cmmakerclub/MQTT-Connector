
#define ON  "0"
#define OFF "1"

PubSubClient::callback_t on_message_arrived =
    [&](const MQTT::Publish & pub) -> void {
      // MQTT SUBSCRIBE
        Serial.println(pub.payload_string());
        if (pub.payload_string() == OFF)
        {
            Serial.println("ON ..");
            digitalWrite(RELAY_PIN, LOW);
            state = true;
            Serial.println(state);
        }
        else if (pub.payload_string() == ON)
        {
            Serial.println("OFF ..");
            digitalWrite(RELAY_PIN, HIGH);
            state = false;
            Serial.println(state);
        }
        else
        {
            Serial.print(pub.topic());
            Serial.print(" => ");
            Serial.println(pub.payload_string());
        }
        Serial.println("");
        Serial.println("");
};
