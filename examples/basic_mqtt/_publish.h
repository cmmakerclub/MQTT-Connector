#include <MqttConnector.h>

extern int pin_state;
extern MqttConnector* mqtt;

static void read_sensor();

float sensor1_value;
float sensor2_value;


#define DEVICE_NAME      "DEVICE_NAME_MUST_BE_CHANGED"
#define DEVICE_NAME_SIZE 40

char myName[DEVICE_NAME_SIZE];

void register_publish_hooks() {
  strcpy(myName, DEVICE_NAME);
  mqtt->on_prepare_data_once([&](void) {
    sensor1_value = -1.0;
    sensor1_value = -1.0;
  });

  mqtt->on_before_prepare_data([&](void) {
    read_sensor();
  });

  mqtt->on_prepare_data([&](JsonObject * root) {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    data["myName"] = myName;
    data["millis"] = millis();
    data["sensor1_value"] = sensor1_value;
    data["sensor1_value"] = sensor2_value;
    data["state"] = pin_state;
  }, PUBLISH_EVERY);

  mqtt->on_after_prepare_data([&](JsonObject * root) {
    /**************
      JsonObject& data = (*root)["d"];
      data.remove("version");
      data.remove("subscription");
    **************/
  });
}

static void read_sensor() {
  sensor1_value = millis()*0.5;
  sensor2_value = millis()*0.75;
}
