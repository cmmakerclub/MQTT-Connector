#include <MqttConnector.h>

extern const char* DEVICE_NAME;

MqttConnector::prepare_data_hook_t on_prepare_data =
[&](JsonObject *root) -> void {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    data["myName"] = DEVICE_NAME;
    data["millis"] = millis();
};

MqttConnector::after_prepare_data_hook_t on_after_prepare_data =
[&](JsonObject *root) -> void {
  /**************
  JsonObject& data = (*root)["d"];
  data.remove("version");
  data.remove("subscription");
  **************/
};
