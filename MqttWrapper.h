#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include "PubSubClient.h"
// #include <Arduino.h>
#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
#include <functional>

#ifndef DEBUG_PRINTER
#define DEBUG_PRINTER Serial
#endif


#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }

#define INFO_PRINT(...) { }
#define INFO_PRINTLN(...) { }

class MqttWrapper
{
public:
    typedef struct
    {
        MQTT::Connect *connOpts;
        PubSubClient *client;
        String* clientId;
        String* channelId;
        String* topicSub;
        String* topicPub;
        String* username;
        String* password;
    } Config;


    typedef void (*callback_t)(void);
    typedef void (*callback_with_arg_t)(void*);
    typedef std::function<void(const MqttWrapper::Config)> cmmc_config_t;
    typedef std::function<void(JsonObject** )> prepare_data_hook_t;
    typedef std::function<void(char* )> publish_data_hook_t;

    MqttWrapper(const char* , uint16_t port = 1883);
    MqttWrapper(const char* , uint16_t port, cmmc_config_t config_hook);
    ~MqttWrapper();

    void init_config(const char*, uint16_t);

    void sync_pub(String payload);

    void connect(PubSubClient::callback_t callback = NULL);
    void _hook_config();
    void set_configuration_hook(cmmc_config_t func)
    {
        _user_hook_config = func;
    }
    void set_prepare_data_hook(prepare_data_hook_t func, unsigned long publish_interval = 3000)
    {
        _user_hook_prepare_data = func;
        _publish_interval = publish_interval;
    }

    void  set_publish_data_hook(publish_data_hook_t func)
    {
        _user_hook_publish_data = func;
    }

    void loop();

protected:
    void _set_default_client_id()
    {
        clientId = ESP.getChipId();

        uint8_t mac[6];
        WiFi.macAddress(mac);
        String result;
        for (int i = 0; i < 6; ++i)
        {
            result += String(mac[i], 16);
            if (i < 5)
                result += ':';
        }

        DEBUG_PRINT("MAC ADDR: ");
        DEBUG_PRINTLN(result);

        channelId = "esp8266/";
        _mac = result;

    }

    const char* getClientId()
    {
        return clientId.c_str();
    }

    void _prepare_data_hook()
    {
        DEBUG_PRINTLN("__CALL BEFORE PUBLISH DATA");

        if (_user_hook_prepare_data != NULL)
        {
            DEBUG_PRINTLN("__user_hook_prepare_data()");
            _user_hook_prepare_data(&root);
        }
        // DEBUG_PRINTLN("BEFORE PUBLISH");
    }

    void _hook_after_publish(char** ptr)
    {
        // DEBUG_PRINTLN("AFTER PUBLISH");
    }

    void doPublish();

protected:

private:
    WiFiClient wclient;
    cmmc_config_t _user_hook_config = NULL;
    prepare_data_hook_t _user_hook_prepare_data = NULL;
    publish_data_hook_t _user_hook_publish_data = NULL;

    String _mqtt_host = "x";
    uint16_t _mqtt_port = 0;
    int _publish_interval = 3000;
    Config _config;

    String clientId = "";
    String topicSub = "";
    String topicPub = "";
    String channelId = "";

    String _username = "";
    String _password = "";
    String _mac = "";

    unsigned int _subscription_counter = 0;

    MQTT::Connect *connOpts = NULL;
    PubSubClient *client = NULL;
    // MQTT::Subscribe()
    //               .add_topic("topic1")

    PubSubClient::callback_t _user_callback = NULL;

    unsigned long prev_millis;

    // const int BUFFER_SIZE = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2);

    StaticJsonBuffer<512> jsonBuffer;
    char jsonStrbuffer[512];
    JsonObject *root;
    JsonObject *d;

    void _connect();
};



#endif//MQTT_WRAPPER_H
