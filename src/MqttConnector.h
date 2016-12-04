#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include <PubSubClient.h>
#include "ESP8266WiFi.h"
#include <functional>
#include <ArduinoJson.h>

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

#define DEBUG_ALL (defined(DEBUG_ESP_CORE) and defined(DEBUG_ESP_SSL) and defined(DEBUG_ESP_WIFI))
// #define DEBUG_ALL 1

#define CMMC_MQTT_DEBUG_MODE 0
#define MQTT_DEBUG_LEVEL_VERBOSE 1


#if CMMC_MQTT_DEBUG_MODE or DEBUG_ALL
    #ifndef DEBUG_ESP_PORT
      #define DEBUG_ESP_PORT Serial
    #endif

    #define MQTT_DEBUG_PRINT(...) { DEBUG_ESP_PORT.print(__VA_ARGS__); }
    #define MQTT_DEBUG_PRINTLN(...) { DEBUG_ESP_PORT.println(__VA_ARGS__); }
#else
    #define MQTT_DEBUG_PRINT(...) { }
    #define MQTT_DEBUG_PRINTLN(...) { }
#endif

enum mqtt_mode_t{
    MODE_PUB_ONLY   = 0b01,
    MODE_SUB_ONLY = 0b10,
    MODE_BOTH = 0b11
};

struct timer {
  int start, interval;
};

class MqttConnector
{
public:
    typedef struct
    {
        MQTT::Connect *connOpts;
        PubSubClient *client;
        mqtt_mode_t mode;
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
        uint16_t mqttPort;
        bool firstCapChannel;
    } Config;


    // #ifdef _GLIBCXX_FUNCTIONAL
    //   typedef std::function<void(const MQTT::Publish&)> callback_t;
    // #else
    //   typedef void(*callback_t)(const MQTT::Publish&);
    // #endif
    typedef void (*callback_t)(void);
    typedef void (*callback_with_arg_t)(void*);
    // typedef std::function<void(const MQTT::Publish&)> callback_t;
    /*
     * Config Hook
     */

    typedef std::function<void(MqttConnector::Config* )> cmmc_config_t;
    typedef std::function<void(MqttConnector::Config)> cmmc_after_config_t;

    /*
    * Connectivity Hook
    */
    typedef std::function<void(int, bool*)> connecting_hook_t;
    typedef std::function<void(void)> connected_hook_t;
    typedef std::function<void(void)> disconnected_hook_t;

    /*
     *  Prepare Data Hook
     */
    typedef std::function<void(JsonObject* )> prepare_data_hook_t;
    typedef std::function<void(void)> before_prepare_data_hook_t;
    typedef before_prepare_data_hook_t before_prepare_data_once_t;

    typedef before_prepare_data_once_t before_message_arrived_once_t;
    typedef std::function<void(JsonObject *)> after_prepare_data_hook_t;

    /*
     * Publish Hook
     */
    typedef std::function<void(char* )> publish_hook_t;
    typedef std::function<void(void)> before_publish_hook_t;
    typedef PubSubClient::callback_t after_publish_hook_t;

    typedef std::function<void(MQTT::Subscribe*)> subscribe_hook_t;
    typedef std::function<void(String, String, String)> after_message_arrived_t;

    MqttConnector(const char* , uint16_t port = 1883);
    MqttConnector(const char* , uint16_t port, cmmc_config_t config_hook);
    ~MqttConnector();

    void _hook_config();
    void loop();
    void init_config(const char*, uint16_t);
    void sync_pub(String payload);
    void clear_last_will(String payload);
    void connect();
    void publish(MQTT::Publish p) {
      if (_config.client != NULL) {
        _config.client->publish(p);
      }
    };

    void publish(String topic, String payload, bool retain) {
      if (_config.client != NULL) {
        MQTT::Publish newpub(topic.c_str(), (uint8_t*)payload.c_str(), payload.length());
        newpub.set_retain(true);
        _config.client->publish(newpub);
      }
    };

    void on_message(PubSubClient::callback_t callback = NULL);
    void on_after_message_arrived(after_message_arrived_t callback = NULL) {
      _user_on_after_message_arrived = callback;
    };

    void on_before_message_arrived_once(before_message_arrived_once_t callback = NULL) {
      _user_on_before_message_arrived_once = callback;
    };

    // void before_publish(before_prepare_data_hook_t callback = NULL);
    // void after_publish(before_prepare_data_hook_t callback = NULL);
    // backward compat
    void on_published(PubSubClient::callback_t callback = NULL);
    void on_after_publish(PubSubClient::callback_t callback = NULL);
    void on_prepare_configuration(cmmc_config_t func);
    void on_after_prepare_configuration(cmmc_after_config_t func);
    void on_connecting(connecting_hook_t cb);
    // void on_connnected(cmmc_config_t cb);
    void on_after_prepare_data(after_prepare_data_hook_t func);
    void on_subscribe(subscribe_hook_t func);
    void on_prepare_data_once(before_prepare_data_once_t func);
    void on_prepare_data(prepare_data_hook_t func, unsigned long publish_interval = 30 *1000);
    void on_before_prepare_data(before_prepare_data_hook_t func);


    void mode(mqtt_mode_t mode) {
        _mode = mode;
    }


protected:
    void _set_default_client_id()
    {
        MQTT_DEBUG_PRINTLN("SET_DEFAULT_MQTT_CLIENT_ID");
        _config.clientId = ESP.getChipId();
        uint8_t mac[6];
        String result = WiFi.macAddress();
        result.toLowerCase();

        MQTT_DEBUG_PRINT("MAC ADDR: ");
        MQTT_DEBUG_PRINT(result);
        MQTT_DEBUG_PRINT("\n");

        _config.channelPrefix = "esp8266";
        _mac = result;

        MQTT_DEBUG_PRINTLN("/* END SET_DEFAULT_MQTT_CLIENT_ID */");

    }

    void doPublish(bool force = false);


private:
    /*
     * Callbacks
     * Hooks
    */
    cmmc_config_t             _user_hook_config = NULL;
    cmmc_after_config_t       _user_hook_after_config = NULL;
    subscribe_hook_t          _user_hook_subscribe = NULL;
    prepare_data_hook_t       _user_hook_prepare_data = NULL;
    after_prepare_data_hook_t _user_hook_after_prepare_data= NULL;

    publish_hook_t            _user_hook_publish_data = NULL;
    connecting_hook_t         _user_hook_connecting = NULL;

    PubSubClient::callback_t  _on_message_arrived = NULL;
    PubSubClient::callback_t  _user_on_message_arrived = NULL;
    after_message_arrived_t   _user_on_after_message_arrived = NULL;
    before_message_arrived_once_t   _user_on_before_message_arrived_once = NULL;

    after_publish_hook_t      _user_on_after_publish = NULL;
    // on_prepare_data_once
    before_prepare_data_once_t _user_on_prepare_data_once = NULL;
    prepare_data_hook_t        _user_on_prepare_data  = NULL;
    before_prepare_data_hook_t _user_on_before_prepare_data = NULL;
    // before_prepare_data_once_t _user_on_prepare_data_once = NULL;



    /*
     * Methods
    */

    void _clear_last_will();
    void _connect();
    void _hook_after_config();
    // void connecting_hook(int count, bool *flag);
    void _prepare_data_hook();
    void _after_prepare_data_hook();
    void _hook_after_publish(char** ptr);

    struct timer _publish_timer;
    int _timer_expired(struct timer *t) {
        return (millis() - t->start) >= t->interval;
    }
    void _timer_set(struct timer *t, int interval) {
        t->interval = interval;
        t->start = millis();
    }

    /*
     * Attributes
    */
    mqtt_mode_t _mode;
    WiFiClient wclient;
    String _mqtt_host = "";
    uint16_t _mqtt_port = 0;
    Config _config;
    String _mac = "";

    unsigned int _subscription_counter = 0;
    int _publish_interval = 3000;

    MQTT::Subscribe *_subscribe_object;
    MQTT::Publish   *_publish_object;

    unsigned long prev_millis;
    unsigned long _msg_arrived_ms;

    // const int BUFFER_SIZE = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2);
    StaticJsonBuffer<1024> jsonRootBuffer;
    StaticJsonBuffer<512> jsonDBuffer;
    char jsonStrbuffer[1024];

    JsonObject *root;
    JsonObject *d;
    JsonObject *info;

    float _version = 0.96f;
    bool _pub_lock = false;

};

#endif//MQTT_WRAPPER_H
