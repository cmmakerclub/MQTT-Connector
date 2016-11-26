#include <Arduino.h>

String state = "ON";

#define RELAY_PIN 15

static void _receive_run_once() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
}

static void _receive_loop() {
}


PubSubClient::callback_t on_message_arrived =
[&](const MQTT::Publish & pub) -> void {
    String topic = pub.topic();
    String payload = pub.payload_string();
    String text = topic + " => " + payload;
    Serial.println(text);
    if (payload == "ON") {
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(RELAY_PIN, HIGH);
      state = payload;
    }
    else if (payload == "OFF") {
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAY_PIN, LOW);
      state = payload;
    }

 };
