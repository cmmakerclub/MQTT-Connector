#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include "PubSubClient.h"
#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
#include "WiFiConnector.h"
#include <functional>

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif


#define MQTT_DEBUG_MODE

#ifdef MQTT_DEBUG_MODE
#define MQTT_DEBUG_PRINTER Serial
    #define MQTT_DEBUG_PRINT(...) { MQTT_DEBUG_PRINTER.print(__VA_ARGS__); }
    #define MQTT_DEBUG_PRINTLN(...) { MQTT_DEBUG_PRINTER.println(__VA_ARGS__); }
#else
    #define MQTT_DEBUG_PRINT(...) { }
    #define MQTT_DEBUG_PRINTLN(...) { }
#endif

class MqttConnector
{
public:
    typedef struct
    {
        MQTT::Connect *connOpts;
        PubSubClient *client;
        String clientId;
        String channelPrefix;
        String topicSub;
        String topicPub;
        String topicLastWill;
        String username;
        String password;
        String mqttHost;
        bool enableLastWill;
        bool retainPublishMessage;
        bool subscribeOnly;
        bool publishOnly;
        uint16_t mqttPort;
    } Config;


    typedef void (*callback_t)(void);
    typedef void (*callback_with_arg_t)(void*);
    typedef std::function<void(MqttConnector::Config* )> cmmc_config_t;
    typedef std::function<void(MqttConnector::Config)> cmmc_after_config_t;
    typedef std::function<void(JsonObject* )> prepare_data_hook_t;
    typedef std::function<void(JsonObject* )> after_prepare_data_hook_t;
    typedef std::function<void(int, bool*)> connecting_hook_t;
    typedef std::function<void(char* )> publish_data_hook_t;
    typedef std::function<void(MQTT::Subscribe*)> prepare_subscribe_hook_t;

    MqttConnector(const char* , uint16_t port = 1883);
    MqttConnector(const char* , uint16_t port, cmmc_config_t config_hook);
    ~MqttConnector();

    void _hook_config();
    void loop();
    void loop(WiFiConnector *);    
    void init_config(const char*, uint16_t);
    void sync_pub(String payload);
    void clear_last_will(String payload);
    void connect();

    void on_message(PubSubClient::callback_t callback = NULL);
    void on_published(PubSubClient::callback_t callback = NULL);

    void prepare_configuration(cmmc_config_t func)
    {
        _user_hook_config = func;
    }

    void after_prepare_configuration(cmmc_after_config_t func)
    {
        _user_hook_after_config = func;
    }
    
    void on_connecting(connecting_hook_t cb) {
        _user_hook_connecting = cb;
    }

    void prepare_data(prepare_data_hook_t func, unsigned long publish_interval = 30 *1000)
    {
        _user_hook_prepare_data = func;
        _publish_interval = publish_interval;
        _timer_set(&publish_timer, publish_interval);
    }

    void after_prepare_data(after_prepare_data_hook_t func)
    {
        _user_hook_after_prepare_data = func;
    }

    void prepare_subscribe(prepare_subscribe_hook_t func)
    {
        _user_hook_prepare_subscribe = func;
    }

    void set_publish_data_hook(publish_data_hook_t func)
    {
        _user_hook_publish_data = func;
    }
    


protected:
    void _set_default_client_id()
    {
        _config.clientId = ESP.getChipId();
        uint8_t mac[6];
        String result = WiFi.macAddress();
        result.toLowerCase();

        MQTT_DEBUG_PRINT("MAC ADDR: ");
        MQTT_DEBUG_PRINTLN(result);

        _config.channelPrefix = "esp8266";
        _mac = result;

    }

    void connecting_hook(int count, bool *flag) {
        if (_user_hook_connecting != NULL) {
            _user_hook_connecting(count, flag);
        }
    }

    void _prepare_data_hook()
    {

        if (_user_hook_prepare_data != NULL)
        {
            MQTT_DEBUG_PRINTLN("__user_hook_prepare_data()");
            _user_hook_prepare_data(root);
        }

    }

    void _after_prepare_data_hook()
    {
        if (_user_hook_after_prepare_data != NULL)
        {
            MQTT_DEBUG_PRINTLN("__user_hook_after_prepare_data()");
            _user_hook_after_prepare_data(root);
        }
        
        // MQTT_DEBUG_PRINTLN("BEFORE PUBLISH");
    }


    void _hook_after_publish(char** ptr)
    {
        // MQTT_DEBUG_PRINTLN("AFTER PUBLISH");
    }

    void doPublish(bool force = false);

protected:

private:
    WiFiClient wclient;

    // hooks
    cmmc_config_t             _user_hook_config = NULL;
    cmmc_after_config_t       _user_hook_after_config = NULL;
    prepare_subscribe_hook_t  _user_hook_prepare_subscribe = NULL;
    prepare_data_hook_t       _user_hook_prepare_data = NULL;
    after_prepare_data_hook_t _user_hook_after_prepare_data= NULL;

    publish_data_hook_t       _user_hook_publish_data = NULL;
    connecting_hook_t         _user_hook_connecting = NULL;

    PubSubClient::callback_t _on_message_arrived = NULL;
    PubSubClient::callback_t _user_on_message_arrived = NULL;
    PubSubClient::callback_t _user_on_published = NULL;

    void _clear_last_will(); 

    String _mqtt_host = "";
    uint16_t _mqtt_port = 0;

    Config _config;
    String _mac = "";

    unsigned int _subscription_counter = 0;
    int _publish_interval = 3000;    

    MQTT::Subscribe *_subscribe_object;
    MQTT::Publish   *_publish_object;


    unsigned long prev_millis;

    // const int BUFFER_SIZE = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2);

    StaticJsonBuffer<800> jsonRootBuffer;
    StaticJsonBuffer<512> jsonDBuffer;
    // StaticJsonBuffer<128> jsonInfoBuffer;

    char jsonStrbuffer[1024];
    JsonObject *root;
    JsonObject *d;
    JsonObject *info;

    PubSubClient *client;
    
    String _version = "0.23";

    struct timer { int start, interval; };
    struct timer publish_timer;
    
    int _timer_expired(struct timer *t) {
        return (millis() - t->start) >= t->interval; 
    }  

    void _timer_set(struct timer *t, int interval) { 
        t->interval = interval; 
        t->start = millis();
    }

    bool _pub_lock = false;

    void _connect();
};



#endif//MQTT_WRAPPER_H
