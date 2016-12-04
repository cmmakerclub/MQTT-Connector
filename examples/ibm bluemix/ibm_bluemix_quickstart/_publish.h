#include <MqttConnector.h>

extern MqttConnector* mqtt;

#define DEVICE_NAME      "DEVICE_NAME_MUST_BE_CHANGED"
#define DEVICE_NAME_SIZE 40

char myName[DEVICE_NAME_SIZE];

//forward declaration
static void read_sensor();

void register_publish_hooks() {
  mqtt->on_prepare_data_once([&](void) {
    strcpy(myName, DEVICE_NAME);
  });

  mqtt->on_before_prepare_data([&](void) {
    read_sensor();
  });

  mqtt->on_prepare_data([&](JsonObject * root) {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    data["myName"] = myName;
    data["millis"] = millis();
  }, PUBLISH_EVERY);

  mqtt->on_after_prepare_data([&](JsonObject * root) {
    root->remove("info");
    /**************
      JsonObject& data = (*root)["d"];
      data.remove("version");
      data.remove("subscription");
    **************/
  });
}

static void read_sensor() {
  // SET SHARED VARIABLE
}