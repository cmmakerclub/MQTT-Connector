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
float t,h,p;

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
    data["temperature"] = t;
    data["humidity"] = h;
    data["pressure"] = p;
    data["relayState"] = relayPinState;
    data["updateInterval"] = PUBLISH_EVERY;
    data["A0"] = analogRead(A0);
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
  t = String(bme.readTemperature()).toFloat();
  h = String(bme.readHumidity()).toFloat(); 
  p = String(bme.readPressure()).toFloat(); 

  if ((isnan(h) || h == 0)  ||  (isnan(t) || t == 0)) {
    Serial.println("read bme280 failed... try again..");
  }
  else {
    Serial.printf("Temperature: %.2f, Humidity: %.2f, Sensor Pressure = %.2f\r\n", t, h, p); 
  } 

}
