#include <MqttConnector.h>

/* DEVICE DATA & FREQUENCY */
const char *DEVICE_NAME       = "plug001";
const int PUBLISH_EVERY       = 3000;

#define DHTPIN 12     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
CMMC_Interval timer001;

static float t_dht = 0;
static float h_dht = 0;

static void read_dht();
static void _publish_run_once() {
  dht.begin();
  read_dht();
}

static void _publish_loop() {
  timer001.every_ms(2000, read_dht);
}

extern String state;
static void read_dht() {
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

MqttConnector::prepare_data_hook_t on_prepare_data =
[&](JsonObject *root) -> void {
  JsonObject& data = (*root)["d"];
  JsonObject& info = (*root)["info"];

  data["myName"] = DEVICE_NAME;
  data["tags"] = "inside,cmmc,plug";
  data["temp"] = t_dht;
  data["humid"] = h_dht;
  data["state"] = state;
};
