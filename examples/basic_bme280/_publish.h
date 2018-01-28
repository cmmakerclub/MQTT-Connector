#include <MqttConnector.h>
#include <Adafruit_BME280.h>


extern int relayPinState;
extern MqttConnector* mqtt;
extern int relayPin;
extern char myName[];

static void readSensor(); 

extern String DEVICE_NAME;
extern int PUBLISH_EVERY;
extern Adafruit_BME280 bme;

void register_publish_hooks() {
  strcpy(myName, DEVICE_NAME.c_str());
  mqtt->on_prepare_data_once([&](void) {
    Serial.println("initializing sensor...");
  });

  mqtt->on_before_prepare_data([&](void) {
    readSensor();
  });

  mqtt->on_prepare_data([&](JsonObject *root) {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    data["myName"] = myName;
    data["millis"] = millis();
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
  float t = bme.readTemperature();
  float h = bme.readHumidity(); 
  float p_raw = bme.readPressure(); 

  if ((isnan(h) || h == 0)  ||  (isnan(t) || t == 0)) {
    Serial.println("read bme280 failed... try again..");
  }
  else {
    String tStr = String(t);
    String hStr = String(h);
    Serial.printf("Temperature: %s, Humidity: %s\r\n", tStr.c_str(), hStr.c_str());
  } 

}
