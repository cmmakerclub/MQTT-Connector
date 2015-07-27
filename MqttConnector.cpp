#include "MqttConnector.h"

MqttConnector::MqttConnector(const char* host, uint16_t port)
{
    init_config(host, port);
    MQTT_DEBUG_PRINTLN("----------- Wrapper CONSTRUCTOR ---------");
    MQTT_DEBUG_PRINT(_mqtt_host);
    MQTT_DEBUG_PRINT(" - ");
    MQTT_DEBUG_PRINT(_mqtt_port);
    MQTT_DEBUG_PRINTLN("---------- /Wrapper CONSTRUCTOR ---------");
}

void MqttConnector::init_config(const char* host, uint16_t port)
{
    prev_millis = millis();

    _mqtt_host = String(host);
    _mqtt_port = port;

    connOpts = NULL;
    client = NULL;
    
    _subscribe_object = NULL;

    JsonObject& r = jsonBuffer.createObject();
    JsonObject& d = jsonBuffer.createObject();

    this->root = &r;
    this->d = &d;

    r["d"] = d;
}

MqttConnector::MqttConnector(const char* host, uint16_t port, cmmc_config_t config_hook)
{
    init_config(host, port);
    _user_hook_config = config_hook;

}

void MqttConnector::connect(PubSubClient::callback_t callback)
{
    MQTT_DEBUG_PRINTLN("BEGIN Wrapper");

    _set_default_client_id();

    _hook_config();

    if (callback != NULL)
    {
        MQTT_DEBUG_PRINTLN("__USER REGISTER SUBSCRIPTION CALLBACK");
        _user_callback = callback;

        client->set_callback([&](const MQTT::Publish& pub)
        {
            if (_user_callback != NULL)
            {
                MQTT_DEBUG_PRINTLN("CALLING USER SUBSCRIPTION CALLBACK...");
                _user_callback(pub);
            }
        });
    }
    else
    {
        MQTT_DEBUG_PRINTLN("__USER DOES NOT REGISTER SUBSCRIPTION CALLBACk");
    }

    _connect();

}


void MqttConnector::_hook_config()
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
        MQTT_DEBUG_PRINTLN("OVERRIDE CONFIG IN _hook_config");
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
        MQTT_DEBUG_PRINTLN("HOOK CONFIG SKIPPED. USE DEFAULT!");
    }



    if (_user_callback != NULL)
    {
        MQTT_DEBUG_PRINT("__SUBSCRIPTION TOPIC -> ");
        MQTT_DEBUG_PRINTLN(topicSub)
    }
    else
    {
    }


    MQTT_DEBUG_PRINT("__PUBLICATION TOPIC -> ");

    MQTT_DEBUG_PRINTLN(topicPub)

    MQTT_DEBUG_PRINT("__SUBSCRIPTION TOPIC -> ");

    MQTT_DEBUG_PRINTLN(topicSub);

    connOpts = new MQTT::Connect(clientId);
    client = new PubSubClient(wclient);
  
    client->set_server(_mqtt_host, _mqtt_port);
    connOpts->set_auth(_username, _password);
}

void MqttConnector::sync_pub(String payload)
{
    MQTT_DEBUG_PRINT("SYNC PUB.... -> ");
    MQTT_DEBUG_PRINTLN(payload.c_str());
    MQTT::Publish newpub(topicSub, (uint8_t*)payload.c_str(), payload.length());
    newpub.set_retain(true);
    client->publish(newpub);
}

void MqttConnector::loop()
{
    if (client->loop())
    {
        doPublish();
    }
    else
    {
        MQTT_DEBUG_PRINTLN("MQTT DISCONNECTED");
        _connect();
    }

}

void MqttConnector::loop(WiFiConnector *wifiHelper)
{
    wifiHelper->loop();
    this->loop();
}



void MqttConnector::doPublish()
{
    static long counter = 0;
    char *dataPtr = NULL;
    if (millis() - prev_millis > _publish_interval && client->connected())
    {

        _prepare_data_hook();
        (*d)["counter"] = ++counter;
        (*d)["heap"] = ESP.getFreeHeap();
        (*d)["seconds"] = millis()/1000;
        (*d)["sub_counter"] = _subscription_counter;     
        (*d)["version"] = _version;                


        strcpy(jsonStrbuffer, "");
        root->printTo(jsonStrbuffer, sizeof(jsonStrbuffer));
        dataPtr = jsonStrbuffer;
        prev_millis = millis();

        MQTT_DEBUG_PRINTLN("__DO PUBLISH ");
        MQTT_DEBUG_PRINT("______________ TOPIC: -->");
        MQTT_DEBUG_PRINTLN(topicPub);
        MQTT_DEBUG_PRINT("______________ CONTENT: -->");
        MQTT_DEBUG_PRINTLN(jsonStrbuffer);

        if (_user_hook_publish_data != NULL)
        {
            _user_hook_publish_data(dataPtr);
        }

        MQTT::Publish newpub(topicPub, (uint8_t*)jsonStrbuffer, strlen(jsonStrbuffer));
        newpub.set_retain(true);

        if(!client->publish(newpub)) {
            return;
        }

        MQTT_DEBUG_PRINT(dataPtr);
        MQTT_DEBUG_PRINTLN(" PUBLISHED!");
        _hook_after_publish(&dataPtr);
    }
}

void MqttConnector::_connect()
{
    MQTT_DEBUG_PRINTLN("== Wrapper.connect(); CONNECT WITH OPTIONS = ");
    MQTT_DEBUG_PRINT("HOST: ");
    MQTT_DEBUG_PRINTLN(_mqtt_host);
    MQTT_DEBUG_PRINT("PORT: ");
    MQTT_DEBUG_PRINTLN(_mqtt_port);
    MQTT_DEBUG_PRINT("clientId: ");
    MQTT_DEBUG_PRINTLN(clientId);


    client->set_max_retries(150);
    while(!client->connect(*connOpts))
    {
        MQTT_DEBUG_PRINTLN("KEEP CONNECTING...");
        delay(100);
    }

    MQTT_DEBUG_PRINTLN("CONNECTED");

    if (_user_callback != NULL)
    {
        MQTT_DEBUG_PRINT("__SUBSCRIBING... ->");
        MQTT_DEBUG_PRINTLN(topicSub);
        delete _subscribe_object;
        _subscribe_object = new MQTT::Subscribe(topicSub);
        if (client->subscribe(*_subscribe_object)) {
            _subscription_counter++;
        }
        else {
            return;
        }

        MQTT_DEBUG_PRINT("__SUBSCRIBED TO ");
        MQTT_DEBUG_PRINTLN(topicSub);
        
    }
    else
    {
        MQTT_DEBUG_PRINTLN("__ PUBLISH ONLY MODE");
    }
}


MqttConnector::~MqttConnector()
{
    delete connOpts;
    delete client;
    delete _subscribe_object;
    connOpts = NULL;
    client = NULL;
    _subscribe_object = NULL;
}
