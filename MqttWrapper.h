#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include "ArduinoJson.h"
#include "PubSubClient.h"
// #include <Arduino.h>
#include <functional>
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




class MqttWrapper
{
public:
    typedef struct { 
        MQTT::Connect *connOpts;
        PubSubClient *client;
        String* clientId;
    } Config;

    typedef void (*callback_t)(void);
    typedef void (*callback_with_arg_t)(void*);
    typedef std::function<void(const MqttWrapper::Config&)> cmmc_config_t;

    MqttWrapper(const char* , int port = 1883);
    MqttWrapper(const char* , int port, cmmc_config_t config_hook);
    ~MqttWrapper();

    void set_callback(PubSubClient::callback_t callback) {
        _user_callback = callback;
    }

    void connect() {
        DEBUG_PRINTLN("Wrapper.connect(); CONNECT WITH OPTIONS = ");
        DEBUG_PRINT("HOST: ");
        DEBUG_PRINT(_mqtt_host);
        DEBUG_PRINT(" PORT: ");
        DEBUG_PRINT(_mqtt_port);

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
        Serial.println("BEGIN Wrapper");
        setDefaultClientId();
        client = new PubSubClient(_mqtt_host, _mqtt_port);
        connOpts = new MQTT::Connect(clientId);

        client->set_callback([&](const MQTT::Publish& pub) {
            DEBUG_PRINTLN("DEFAULT CALL BACK .MqttWrapper.");
            if (_user_callback != NULL) {
                _user_callback(pub);
            }
        });

        hook_config();
    }

    void hook_config() {
        Config c;
        c.connOpts = connOpts;
        c.client = client;
        c.clientId = &(this->clientId);

        Serial.println("DOING HOOKCONFIG");
        if (_user_hook_config != NULL) {
            Serial.println("IN HOOK CONFIG");
           _user_hook_config(c);
        }
        else {
            Serial.println("NOT IN NOT IN HOOK CONFIG");
        }
    }

    void set_config_hook(cmmc_config_t func) {
        _user_hook_config = func;
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
    cmmc_config_t _user_hook_config;

    String clientId;
    String topicId;

    MQTT::Connect *connOpts;
    PubSubClient *client;



    PubSubClient::callback_t _callback;
    PubSubClient::callback_t _user_callback;

    unsigned long prev_millis;

private:
    String _mqtt_host = "x";
    int _mqtt_port = 0;
};



#endif//MQTT_WRAPPER_H
