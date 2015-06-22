#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <PubSubClient.h>
#include "DHT.h"

// const char* ssid     = "OpenWrt_NAT_500GP.101";
// const char* pass = "activegateway";

// const char* ssid     = "MAKERCLUB-CM";
// const char* pass = "welcomegogogo";

// const char* ssid     = "Opendream Play";
// const char* pass = "5k,skrijv',7'sik";

const char* ssid     = "Opendream";
const char* pass = "gfkgvkgv'2015!!!!";


#define WIFI_MAX_RETRIES 1500
#define WIFI_CONNECT_DELAY_MS 20

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)


MqttWrapper *mqtt;
DHT *dht;

void init_dht(DHT **dht, uint8_t pin, uint8_t dht_type);
void read_dht(DHT *dht, float *temp, float *humid);

static float t_dht;
static float h_dht; 

void callback(const MQTT::Publish& pub) {
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
}

void connect_wifi()
{
    WiFi.begin(ssid, pass);

    int retries = 0;
    while ((WiFi.status() != WL_CONNECTED))
    {
        Serial.print(".");
        retries++;
        delay(500);
    }

    Serial.println("WIFI CONNECTED ");
}

void hook_before_publish(JsonObject** root) {
  JsonObject& data = (*(*root))["d"];
  read_dht(dht, &t_dht, &h_dht);

  data["myName"] = "NAT";
  data["temp"] = t_dht;
  data["humid"] = h_dht;
}

void hook_publish_data(char** dataPtr) {
}

void hook_configuration(MqttWrapper::Config config) {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    String result;
    for (int i = 0; i < 6; ++i)
    {
        result += String(mac[i], 16);
        // if (i < 5)
        //     result += ':';
    }
    
    *(config.clientId) = String("d:quickstart:arduino:") + result;
    *(config.topicPub) = "iot-2/evt/status/fmt/json";


    Serial.println("+++++++++++++++++++");
    Serial.println(*(config.clientId));
    Serial.println("IN HOOK CONFIG INO");
    Serial.print("CLIENT ID: --> ");
    Serial.println(*(config.clientId));
    Serial.print("topicPub: --> ");
    Serial.println(*(config.topicPub));
    Serial.println("-------------------");
}



void setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println("GO");

    init_dht(&dht, DHTPIN, DHTTYPE);

    connect_wifi();

    // mqtt = new MqttWrapper("128.199.104.122", 1883, hook_configuration);
    mqtt = new MqttWrapper("quickstart.messaging.internetofthings.ibmcloud.com", 1883, hook_configuration);
    mqtt->connect(callback);
    mqtt->set_prepare_publish_data_hook(hook_before_publish);
}

void loop() {
    mqtt->loop();
}


void init_dht(DHT **dht, uint8_t pin, uint8_t dht_type) {
    // Connect pin 1 (on the left) of the sensor to +5V
    // NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
    // to 3.3V instead of 5V!
    // Connect pin 2 of the sensor to whatever your DHTPIN is
    // Connect pin 4 (on the right) of the sensor to GROUND
    // Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

    // Initialize DHT sensor for normal 16mhz Arduino
    // NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
    // might need to increase the threshold for cycle counts considered a 1 or 0.
    // You can do this by passing a 3rd parameter for this threshold.  It's a bit
    // of fiddling to find the right value, but in general the faster the CPU the
    // higher the value.  The default for a 16mhz AVR is a value of 6.  For an
    // Arduino Due that runs at 84mhz a value of 30 works.
    // Example to initialize DHT sensor for Arduino Due:
    //DHT dht(DHTPIN, DHTTYPE, 30);

    *dht = new DHT(pin, dht_type, 30);
    (*dht)->begin();
    DEBUG_PRINTLN(F("DHTxx test!"))  ;
}


void read_dht(DHT *dht, float *temp, float *humid) {

    if (dht == NULL) {
        DEBUG_PRINTLN(F("[dht22] is not initialised. please call init_dht() first."));
        return;
    }

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht->readHumidity();

    // Read temperature as Celsius
    float t = dht->readTemperature();
    // Read temperature as Fahrenheit
    float f = dht->readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
        DEBUG_PRINTLN("Failed to read from DHT sensor!");
        return;
    }

    // Compute heat index
    // Must send in temp in Fahrenheit!
    float hi = dht->computeHeatIndex(f, h);

    DEBUG_PRINT("Humidity: ");
    DEBUG_PRINT(h);
    DEBUG_PRINT(" %\t");
    DEBUG_PRINT("Temperature: ");
    DEBUG_PRINT(t);
    DEBUG_PRINT(" *C ");
    DEBUG_PRINT(f);
    DEBUG_PRINT(" *F\t");
    DEBUG_PRINT("Heat index: ");
    DEBUG_PRINT(hi);
    DEBUG_PRINTLN(" *F");

    *temp = t;
    *humid = f;

}