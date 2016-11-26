#include <MqttConnector.h>

extern String DEVICE_NAME;

MqttConnector::before_prepare_data_once_t
on_prepare_data_once =[&](void) -> void {

};

MqttConnector::before_prepare_data_hook_t
on_before_prepare_data_loop = [&](void) -> void {

};

MqttConnector::prepare_data_hook_t on_prepare_data =
[&](JsonObject *root) -> void {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    data["myName"] = DEVICE_NAME;
    data["millis"] = millis();
};

MqttConnector::after_prepare_data_hook_t
on_after_prepare_data = [&](JsonObject *root) -> void {
  /**************
  JsonObject& data = (*root)["d"];
  data.remove("version");
  data.remove("subscription");
  **************/
};
