#include <MqttConnector.h>

static unsigned int min;
static unsigned int max;

MqttConnector::prepare_data_hook_t on_prepare_data =
[&](JsonObject * root) -> void {

    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];

    data["myName"] = DEVICE_NAME;
    data["door_open"] = door_open;
    // if (current_status)
    // data["filterd"] = map(data["adc"], min+30, max, 0, 100);

};
