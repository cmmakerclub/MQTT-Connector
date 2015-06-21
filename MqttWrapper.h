#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

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
    typedef std::function<void(const MqttWrapper::Config)> cmmc_config_t;
    typedef std::function<void(char** payloadPtr)> publish_hook_t;

    MqttWrapper(const char* , int port = 1883);
    MqttWrapper(const char* , int port, cmmc_config_t config_hook);
    ~MqttWrapper();

    void initConfig(const char*, int);
    void connect(PubSubClient::callback_t callback) {
        Serial.println("BEGIN Wrapper");

        setDefaultClientId();

        client = new PubSubClient(_mqtt_host, _mqtt_port);
        connOpts = new MQTT::Connect(clientId);

        _user_callback = callback;

        client->set_callback([&](const MQTT::Publish& pub) {
            if (_user_callback != NULL) {
                _user_callback(pub);
            }
        });

        hook_config();
    }

    void hook_config() {
        _config.connOpts = connOpts;
        _config.client = client;
        _config.clientId = &(this->clientId);

        Serial.println("DOING HOOKCONFIG");
        if (_user_hook_config != NULL) {
            Serial.println("IN HOOK CONFIG");
           _user_hook_config(_config);
        }
        else {
            Serial.println("NOT IN NOT IN HOOK CONFIG");
        }
    }

    void set_config_hook(cmmc_config_t func) {
        _user_hook_config = func;
    }

    void set_before_publish_hook(publish_hook_t func) {
        _user_hook_before_publish = func;
    }


    void loop() {
        if (client->loop())
        {
            doPublish();
        }
        else 
        {
            DEBUG_PRINTLN("MQTT DISCONNECTED");
            _connect();
        }

    }

protected:
    void setDefaultClientId() {
        clientId = ESP.getChipId();
        topic_sub = "esp8266-18:fe:34:a0:7e:58/data";
        topic_pub = "sampleTopic";
    }

    const char* getClientId()
    {
        return clientId.c_str();
    }

    void beforePublish(char** ptr) {
        DEBUG_PRINTLN("__BEFORE__");
        if (_user_hook_before_publish != NULL) {
            DEBUG_PRINTLN("__BEFORE__OK___");
            _user_hook_before_publish(ptr);
        }
        // DEBUG_PRINTLN("BEFORE PUBLISH");
    }

    void afterPublish() {
        // DEBUG_PRINTLN("AFTER PUBLISH");
    }

    void doPublish() {
        char *dataPtr;
        if (millis() - prev_millis > 3000) {
            beforePublish(&dataPtr);
            prev_millis = millis();
            DEBUG_PRINT("DO PUBLISH --> ");
            while (!client->publish(topic_pub, dataPtr))
            {
                DEBUG_PRINTLN("PUBLISHED ERROR.");
            }
            DEBUG_PRINT(dataPtr);
            DEBUG_PRINTLN(" PUBLISHED!");
            afterPublish();
        }
    }




protected:

private:
    cmmc_config_t _user_hook_config;
    publish_hook_t _user_hook_before_publish;

    String _mqtt_host = "x";
    int _mqtt_port = 0;
    Config _config;

    String clientId;
    String topic_sub;
    String topic_pub;

    MQTT::Connect *connOpts;
    PubSubClient *client;

    PubSubClient::callback_t _user_callback;

    unsigned long prev_millis;
 

    void _connect() {
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

        DEBUG_PRINT("SUBSCRIBING...");
        DEBUG_PRINTLN(topic_sub);

        while(!client->subscribe(topic_sub)) {
            DEBUG_PRINT("subscribing...");
            DEBUG_PRINTLN(topic_sub);
            delay(100);
        };

        DEBUG_PRINTLN("subscribeed");

    }

};



#endif//MQTT_WRAPPER_H
