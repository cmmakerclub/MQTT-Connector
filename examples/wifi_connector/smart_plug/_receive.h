#include <MqttConnector.h>

PubSubClient::callback_t on_message_arrived =
[&](const MQTT::Publish & pub) -> void {

  String topic = pub.topic();
  String payload = pub.payload_string();
  String text = topic + " => " + payload;
  Serial.println(payload);
  Serial.println(payload.toInt());

  if (topic.lastIndexOf("/command") != -1) {
     for (int i = 0; i < payload.length(); i++) {
       Serial.print(payload.charAt(i));
       Serial.print(" ");
       Serial.print(payload.charAt(i), BIN);

       pin01_state = payload.charAt(i) & RELAY_01_MASK;
       pin02_state = payload.charAt(i) & RELAY_02_MASK;
       Serial.printf("Result: %d,%d\r\n", pin01_state, pin02_state);

       digitalWrite(RELAY_01_PIN, pin01_state);
       digitalWrite(RELAY_02_PIN, pin02_state);

       EEPROM.write(EEPROM_ADDR, payload.charAt(i));
     }
     EEPROM.commit();
   }
 };
