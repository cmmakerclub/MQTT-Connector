#include <MqttConnector.h>
#include <DHT.h>

extern int relayPinState;
extern MqttConnector* mqtt;
extern int relayPin;
extern char myName[];

static void readSensor();

// sensor
float temperature_c, humidity_percent_rh = 0;
DHT dht(12, DHT22);


extern String DEVICE_NAME;
extern int PUBLISH_EVERY;

void register_publish_hooks() {
  strcpy(myName, DEVICE_NAME.c_str());
  mqtt->on_prepare_data_once([&](void) {
    Serial.println("initializing sensor...");
    dht.begin();
  });

  mqtt->on_before_prepare_data([&](void) {
    readSensor();
  });

  mqtt->on_prepare_data([&](JsonObject *root) {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    data["myName"] = myName;
    data["millis"] = millis();
    data["temperature_c"] = temperature_c;
    data["humidity_percent_rh"] = humidity_percent_rh;
    data["state"] = relayPinState;
  }, PUBLISH_EVERY);

  mqtt->on_after_prepare_data([&](JsonObject * root) {
    /**************
      JsonObject& data = (*root)["d"];
      data.remove("version");
      data.remove("subscription");
    **************/
  });
}

static void readSensor() {
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
    temperature_c = t;
    humidity_percent_rh = h;
    Serial.print("Temp: ");
    Serial.println(temperature_c);
    Serial.print("Humid: ");
    Serial.println(humidity_percent_rh);
  }

}
