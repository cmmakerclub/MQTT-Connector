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


#ifdef DEBUG_MODE
#ifdef DEBUG_LEVEL_VERBOSE
#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }

#define INFO_PRINT(...) { }
#define INFO_PRINTLN(...) { }
#else
#define DEBUG_LEVEL_INFO
#endif

#ifdef DEBUG_LEVEL_INFO
#define DEBUG_PRINT(...) {}
#define DEBUG_PRINTLN(...) {}

#define INFO_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define INFO_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#endif
#else
#define DEBUG_PRINT(...) {}
#define DEBUG_PRINTLN(...) {}

#define INFO_PRINT(...) { }
#define INFO_PRINTLN(...) { }
#endif




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

    void sync_pub(String payload)
    {
        DEBUG_PRINT("SYNC PUB.... -> ");
        INFO_PRINT("SYNC PUB.... -> ");
        DEBUG_PRINTLN(payload.c_str());
        INFO_PRINTLN(payload.c_str());
        MQTT::Publish newpub(topicSub, (uint8_t*)payload.c_str(), payload.length());
        newpub.set_retain(true);
        client->publish(newpub);
    }

    void connect(PubSubClient::callback_t callback = NULL)
    {
        DEBUG_PRINTLN("BEGIN Wrapper");

        _set_default_client_id();

        _hook_config();

        if (callback != NULL)
        {
            DEBUG_PRINTLN("__USER REGISTER SUBSCRIPTION CALLBACK");
            _user_callback = callback;

            client->set_callback([&](const MQTT::Publish& pub)
            {
                if (_user_callback != NULL)
                {
                    DEBUG_PRINTLN("CALLING USER SUBSCRIPTION CALLBACK...");
                    _user_callback(pub);
                }
            });
        }
        else
        {
            DEBUG_PRINTLN("__USER DOES NOT REGISTER SUBSCRIPTION CALLBACk");
        }

        _connect();

    }

    void _hook_config()
    {
        _config.connOpts = connOpts;
        // _config.client = client;

        _config.clientId  = &(this->clientId);
        _config.topicSub  = &(this->topicSub);
        _config.topicPub  = &(this->topicPub);
        _config.channelId = &(this->channelId);
        _config.username  = &(this->_username);
        _config.password  = &(this->_password);


        if (_user_hook_config != NULL)
        {
            DEBUG_PRINTLN("OVERRIDE CONFIG IN _hook_config");
            INFO_PRINTLN("OVERRIDE CONFIG IN _hook_config");
            _user_hook_config(_config);
            if (topicPub.length() == 0 )
            {
                topicPub = channelId + _mac + String("/status");
            }

            if (topicSub.length() == 0)
            {
                topicSub = channelId + _mac + String("/command");
            }

        }
        else
        {
            topicSub = channelId + _mac + String("/command");
            topicPub = channelId + _mac + String("/status");
            DEBUG_PRINTLN("HOOK CONFIG SKIPPED. USE DEFAULT!");
        }



        if (_user_callback != NULL)
        {
            INFO_PRINT("__SUBSCRIPTION TOPIC -> ");
            DEBUG_PRINT("__SUBSCRIPTION TOPIC -> ");
            INFO_PRINTLN(topicSub)
            DEBUG_PRINTLN(topicSub)
        }
        else
        {
        }


        INFO_PRINT("__PUBLICATION TOPIC -> ");
        DEBUG_PRINT("__PUBLICATION TOPIC -> ");

        INFO_PRINTLN(topicPub)
        DEBUG_PRINTLN(topicPub)

        INFO_PRINT("__SUBSCRIPTION TOPIC -> ");
        DEBUG_PRINT("__SUBSCRIPTION TOPIC -> ");

        INFO_PRINTLN(topicSub);
        DEBUG_PRINTLN(topicSub);

        connOpts = new MQTT::Connect(clientId);
        client = new PubSubClient();
        client->set_server(_mqtt_host, _mqtt_port);
        connOpts->set_auth(_username, _password);
    }

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

    void loop()
    {
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

    void doPublish()
    {
        static long counter = 0;
        char *dataPtr = NULL;
        if (millis() - prev_millis > _publish_interval && client->connected())
        {

            _prepare_data_hook();
            (*d)["counter"] = ++counter;
            (*d)["heap"] = ESP.getFreeHeap();
            (*d)["seconds"] = millis()/1000;


            strcpy(jsonStrbuffer, "");
            root->printTo(jsonStrbuffer, sizeof(jsonStrbuffer));
            dataPtr = jsonStrbuffer;
            prev_millis = millis();

            INFO_PRINT("PUBLISH DATA --> ");
            INFO_PRINTLN(jsonStrbuffer);
            DEBUG_PRINTLN("__DO PUBLISH ");
            DEBUG_PRINT("______________ TOPIC: -->");
            DEBUG_PRINTLN(topicPub);
            DEBUG_PRINT("______________ CONTENT: -->");
            DEBUG_PRINTLN(jsonStrbuffer);

            if (_user_hook_publish_data != NULL)
            {
                _user_hook_publish_data(dataPtr);
            }

            MQTT::Publish newpub(topicPub, (uint8_t*)jsonStrbuffer, strlen(jsonStrbuffer));
            newpub.set_retain(false);

            while (!client->publish(newpub))
            {
                DEBUG_PRINTLN("__PUBLISHED KEEP TRYING...");
                INFO_PRINTLN("__PUBLISHED KEEP TRYING...");
                delay(500);
            }
            DEBUG_PRINT(dataPtr);
            DEBUG_PRINTLN(" PUBLISHED!");
            _hook_after_publish(&dataPtr);
        }
    }




protected:

private:
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

    MQTT::Connect *connOpts = NULL;
    PubSubClient *client = NULL;

    PubSubClient::callback_t _user_callback = NULL;

    unsigned long prev_millis;

    // const int BUFFER_SIZE = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2);

    StaticJsonBuffer<512> jsonBuffer;
    char jsonStrbuffer[512];
    JsonObject *root;
    JsonObject *d;



    void _connect()
    {

        DEBUG_PRINTLN("== Wrapper.connect(); CONNECT WITH OPTIONS = ");
        DEBUG_PRINT("HOST: ");
        DEBUG_PRINTLN(_mqtt_host);
        DEBUG_PRINT("PORT: ");
        DEBUG_PRINTLN(_mqtt_port);
        DEBUG_PRINT("clientId: ");
        DEBUG_PRINTLN(clientId);


        client->set_max_retries(150);
        while(!client->connect(*connOpts))
        {
            DEBUG_PRINTLN("KEEP CONNECTING...");
            // INFO_PRINTLN("connecting...");
            delay(100);
        }

        DEBUG_PRINTLN("CONNECTED");
        // INFO_PRINTLN("CONNECTED");


        if (_user_callback != NULL)
        {
            DEBUG_PRINT("__SUBSCRIBING... ->");
            DEBUG_PRINTLN(topicSub);

            while(!client->subscribe(topicSub))
            {
                DEBUG_PRINT("KEEP SUBSCRIBING...");
                DEBUG_PRINTLN(topicSub);

                INFO_PRINT("KEEP SUBSCRIBING...");
                INFO_PRINTLN(topicSub);
                delay(500);
            };
            DEBUG_PRINT("__SUBSCRIBED TO ");
            INFO_PRINT("__SUBSCRIBED TO ");
            INFO_PRINTLN(topicSub);
            DEBUG_PRINTLN(topicSub);
        }
        else
        {
            DEBUG_PRINTLN("__ PUBLISH ONLY MODE");
            INFO_PRINTLN("__ PUBLISH ONLY MODE");
        }
    }

};



#endif//MQTT_WRAPPER_H
