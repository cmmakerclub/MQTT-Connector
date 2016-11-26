#include <MqttConnector.h>

// MqttConnector::before_prepare_data_hook_t on_prepare_data =
// [&](JsonObject * root) -> void {
//
// };

MqttConnector::prepare_data_hook_t on_prepare_data =
[&](JsonObject *root) -> void {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];

    // data["myName"] = DEVICE_NAME;
    // data["millis"] = millis();
};

// MqttConnector::prepare_data_hook_t on_prepare_data = [&](const MQTT::Publish & pub) -> void {
//
// mqtt->on_published([&](const MQTT::Publish & pub) -> void {
//   static int counter = 0;
//   Serial.print("[MQTT] PUBLISHED: COUNTER = ");
//   Serial.println(counter++);
//   if (counter == 10) {
//     // blinker.blink(100, LED_BUILTIN);
//     // delay(1000);
//     // ESP.deepSleep(45 * 1000000);
//   }
//   else {
//     digitalWrite(LED_BUILTIN, LOW);
//     delay(10);
//     digitalWrite(LED_BUILTIN, HIGH);
//   }
// });
