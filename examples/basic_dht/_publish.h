#include <MqttConnector.h>
#include <DHT.h>

extern int pin_state;
extern MqttConnector* mqtt;
static void read_sensor();
float t_dht, h_dht = 0;
DHT dht(12, DHT22);


#define DEVICE_NAME      "DEVICE_NAME_MUST_BE_CHANGED"
#define DEVICE_NAME_SIZE 40

char myName[DEVICE_NAME_SIZE];

void register_publish_hooks() {
  strcpy(myName, DEVICE_NAME);
  mqtt->on_prepare_data_once([&](void) {
    dht.begin();
  });

  mqtt->on_before_prepare_data([&](void) {
    read_sensor();
  });

  mqtt->on_prepare_data([&](JsonObject * root) {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    data["myName"] = myName;
    data["millis"] = millis();
    data["temp"] = t_dht;
    data["humid"] = h_dht;
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
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else {
    t_dht = t;
    h_dht = h;
    Serial.print("Temp: ");
    Serial.println(t_dht);
    Serial.print("Humid: ");
    Serial.println(h_dht);
  }
}
