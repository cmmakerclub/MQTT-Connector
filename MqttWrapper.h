#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include "ArduinoJson.h"
#include "PubSubClient.h"
// #include <Arduino.h>

#define DEBUG_MODE



#ifdef DEBUG_MODE
#ifndef DEBUG_PRINTER
    #define DEBUG_PRINTER Serial
#endif
#define PRODUCTION_MODE

#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define DEBUG_PRINT(...) {}
#define DEBUG_PRINTLN(...) {}
#endif


typedef struct { 
    MQTT::Connect conOpts;
} Config;

class MqttWrapper
{
public:

    typedef void (*callback_t)(void);
    typedef void (*callback_with_arg_t)(void*);

    MqttWrapper(const char* , int port = 1883);
    ~MqttWrapper();

    void set_callback(PubSubClient::callback_t callback) {
        _user_callback = callback;
    }

    void connect() {

        DEBUG_PRINTLN("CONNECT");

        while(!client->connect(*connOpts)) {
            DEBUG_PRINTLN("connecting...");
            delay(100);
        }

        DEBUG_PRINTLN("CONNECTED");

        while(!client->subscribe("inTopic")) {
            DEBUG_PRINTLN("subscribing...");
            delay(100);
        };

        DEBUG_PRINTLN("subscribeed");

    }

    void begin() {

        client = new PubSubClient(_mqtt_host, _mqtt_port);
        connOpts = new MQTT::Connect(clientId);

        client->set_callback([&](const MQTT::Publish& pub) {
            DEBUG_PRINTLN("DEFAULT CALL BACK .MqttWrapper.");
            if (_user_callback != NULL) {
                _user_callback(pub);
            }
        });
    }

    void hook_config() {
        connOpts->set_auth("", "");
    }


    void loop() {
        if (client->loop())
        {
            doPublish();
        }
        else 
        {
            DEBUG_PRINTLN("MQTT DISCONNECTED");
            connect();
        }

    }

protected:
    void setDefaultClientId() {
        clientId = ESP.getChipId();
    }

    const char* getClientId()
    {
        return clientId.c_str();
    }

    void doPublish() {
        if (millis() - prev_millis > 3000) {
            prev_millis = millis();
            DEBUG_PRINTLN("DO PUBLISH");
        }
    }

protected:
    String clientId;
    String topicId;

    MQTT::Connect *connOpts;
    PubSubClient *client;

    String _mqtt_host;
    int _mqtt_port;

    PubSubClient::callback_t _callback;
    PubSubClient::callback_t _user_callback;

    unsigned long prev_millis;
};



#endif//MQTT_WRAPPER_H
