#include "MqttConnector.h"

MqttConnector::MqttConnector(const char* host, uint16_t port)
{
    init_config(host, port);
    DEBUG_PRINTLN("----------- Wrapper CONSTRUCTOR ---------");
    DEBUG_PRINT(_mqtt_host);
    DEBUG_PRINT(" - ");
    DEBUG_PRINT(_mqtt_port);
    DEBUG_PRINTLN("---------- /Wrapper CONSTRUCTOR ---------");
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
        DEBUG_PRINTLN("OVERRIDE CONFIG IN _hook_config");
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
        DEBUG_PRINTLN(topicSub)
    }
    else
    {
    }


    INFO_PRINT("__PUBLICATION TOPIC -> ");
    DEBUG_PRINT("__PUBLICATION TOPIC -> ");

    DEBUG_PRINTLN(topicPub)

    INFO_PRINT("__SUBSCRIPTION TOPIC -> ");
    DEBUG_PRINT("__SUBSCRIPTION TOPIC -> ");

    DEBUG_PRINTLN(topicSub);

    connOpts = new MQTT::Connect(clientId);
    client = new PubSubClient(wclient);
  
    client->set_server(_mqtt_host, _mqtt_port);
    connOpts->set_auth(_username, _password);
}

void MqttConnector::sync_pub(String payload)
{
    DEBUG_PRINT("SYNC PUB.... -> ");
    INFO_PRINT("SYNC PUB.... -> ");
    DEBUG_PRINTLN(payload.c_str());
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
            DEBUG_PRINTLN("MQTT DISCONNECTED");
            _connect();
        }

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


        strcpy(jsonStrbuffer, "");
        root->printTo(jsonStrbuffer, sizeof(jsonStrbuffer));
        dataPtr = jsonStrbuffer;
        prev_millis = millis();

        INFO_PRINT("PUBLISH DATA --> ");
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
        newpub.set_retain(true);

        if(!client->publish(newpub)) {
            return;
        }

        DEBUG_PRINT(dataPtr);
        DEBUG_PRINTLN(" PUBLISHED!");
        _hook_after_publish(&dataPtr);
    }
}

void MqttConnector::_connect()
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
        delay(100);
    }

    DEBUG_PRINTLN("CONNECTED");

    if (_user_callback != NULL)
    {
        DEBUG_PRINT("__SUBSCRIBING... ->");
        DEBUG_PRINTLN(topicSub);
        delete _subscribe_object;
        _subscribe_object = new MQTT::Subscribe(topicSub);
        if (client->subscribe(*_subscribe_object)) {
            _subscription_counter++;
        }
        else {
            return;
        }

        DEBUG_PRINT("__SUBSCRIBED TO ");
        INFO_PRINT("__SUBSCRIBED TO ");
        DEBUG_PRINTLN(topicSub);
        
    }
    else
    {
        DEBUG_PRINTLN("__ PUBLISH ONLY MODE");
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
