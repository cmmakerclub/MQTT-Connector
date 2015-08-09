#include "MqttConnector.h"

MqttConnector::MqttConnector(const char* host, uint16_t port)
{
    _on_message_arrived = [&](const MQTT::Publish& pub) {
        if (_user_on_message_arrived) {
            _user_on_message_arrived(pub);
        }
    };

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

    _config.connOpts = NULL;
    client = NULL;
    
    _subscribe_object = NULL;
    _subscribe_object = new MQTT::Subscribe();

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

void MqttConnector::_clear_last_will() {
    MQTT::Publish newpub("LWT", '\0', 0);
    newpub.set_retain(true);
    client->publish(newpub);
}

void MqttConnector::on_message(PubSubClient::callback_t callback) {
    if (callback != NULL)
    {
        MQTT_DEBUG_PRINTLN("__USER REGISTER SUBSCRIPTION CALLBACK");
        _user_on_message_arrived = callback;
    }
    else
    {
        MQTT_DEBUG_PRINTLN("__USER DOES NOT REGISTER SUBSCRIPTION CALLBACk");
    }
}


void MqttConnector::connect()
{
    MQTT_DEBUG_PRINTLN("BEGIN Wrapper");


    _set_default_client_id();
    _hook_config();

    _connect();

}


void MqttConnector::_hook_config()
{

    if (_user_hook_config != NULL)
    {
        MQTT_DEBUG_PRINTLN("OVERRIDE CONFIG IN _hook_config");
        _user_hook_config(&_config);
        if (_config.topicPub.length() == 0 )
        {
            _config.topicPub = _config.channelId + _mac + String("/status");
        }

        if (_config.topicSub.length() == 0)
        {
            _config.topicSub = _config.channelId + _mac + String("/command");
        }

    }
    else
    {
        _config.topicSub = _config.channelId + _mac + String("/command");
        _config.topicPub = _config.channelId + _mac + String("/status");
        MQTT_DEBUG_PRINTLN("HOOK CONFIG SKIPPED. USE DEFAULT!");
    }

    MQTT_DEBUG_PRINT("__PUBLICATION TOPIC -> ");

    MQTT_DEBUG_PRINTLN(_config.topicPub)

    MQTT_DEBUG_PRINT("__SUBSCRIPTION TOPIC -> ");

    MQTT_DEBUG_PRINTLN(_config.topicSub);

    _config.connOpts = new MQTT::Connect(_config.clientId);
    // connOpts->set_will("LWT", channelId + _mac, 1, true);
    _config.client = new PubSubClient(wclient);
    client = _config.client;
  
    client->set_server(_mqtt_host, _mqtt_port);
    client->set_callback(_on_message_arrived);
    _config.connOpts->set_auth(_config.username, _config.password);
}

void MqttConnector::sync_pub(String payload)
{
    MQTT_DEBUG_PRINT("SYNC PUB.... -> ");
    MQTT_DEBUG_PRINTLN(payload.c_str());

    MQTT::Publish newpub(_config.topicSub, (uint8_t*)payload.c_str(), payload.length());
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

    if (client->connected() && _timer_expired(&publish_timer))
    {

        _timer_set(&publish_timer, _publish_interval);
        _prepare_data_hook();

        (*d)["counter"] = ++counter;
        (*d)["heap"] = ESP.getFreeHeap();
        (*d)["seconds"] = millis()/1000;
        (*d)["sub_counter"] = _subscription_counter;     
        (*d)["version"] = _version.c_str();                
        (*d)["myName"] = _mac.c_str();


        strcpy(jsonStrbuffer, "");
        root->printTo(jsonStrbuffer, sizeof(jsonStrbuffer));
        dataPtr = jsonStrbuffer;
        prev_millis = millis();

        MQTT_DEBUG_PRINTLN("__DO PUBLISH ");
        MQTT_DEBUG_PRINT("______________ TOPIC: -->");
        MQTT_DEBUG_PRINTLN(_config.topicPub);
        MQTT_DEBUG_PRINT("______________ CONTENT: -->");
        MQTT_DEBUG_PRINTLN(jsonStrbuffer);

        if (_user_hook_publish_data != NULL)
        {
            _user_hook_publish_data(dataPtr);
        }

        MQTT::Publish newpub(_config.topicPub, (uint8_t*)jsonStrbuffer, strlen(jsonStrbuffer));
        newpub.set_retain(true);

        if(!client->publish(newpub)) {
            MQTT_DEBUG_PRINTLN("PUBLISHED FAILED!");
            return;
        }

        // _clear_last_will();

        _hook_after_publish(&dataPtr);

        MQTT_DEBUG_PRINTLN("PUBLISHED SUCCEEDED!");
        MQTT_DEBUG_PRINTLN("====================================");
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
    MQTT_DEBUG_PRINTLN(_config.clientId);


    client->set_max_retries(150);
    bool flag = true;

    while(!client->connect(*(_config.connOpts)) && flag);
    {
        MQTT_DEBUG_PRINTLN("KEEP CONNECTING...");
        if (_user_hook_connecting) {
            _user_hook_connecting(&flag);
        }
        else {
            delay(100);
        }
    }

    MQTT_DEBUG_PRINTLN("CONNECTED");
    MQTT_DEBUG_PRINTLN("====================================");
    MQTT_DEBUG_PRINTLN("====================================");

    // _clear_last_will();

    if (_user_hook_prepare_subscribe != NULL)
    {
        MQTT_DEBUG_PRINTLN("CALLING HOOK SUBSCRIBING..");
        _user_hook_prepare_subscribe(_subscribe_object);
        MQTT_DEBUG_PRINTLN("CHECK IF __SUBSCRIBING... ->");

        _subscribe_object->add_topic(_config.topicSub);
        MQTT_DEBUG_PRINTLN("++TRY SUBSCRIBING ++");
        if (client->subscribe(*_subscribe_object)) {
            _subscription_counter++;
            MQTT_DEBUG_PRINT("__SUBSCRIBED TO ");
            MQTT_DEBUG_PRINTLN(_config.topicSub);
        }
        else {
            // goto loop and recheck connectiviy
            return;
        }
    }
    else
    {
        MQTT_DEBUG_PRINTLN("__ PUBLISH ONLY MODE");
    }
}


MqttConnector::~MqttConnector()
{
    delete _config.connOpts;
    delete _config.client;
    delete _subscribe_object;
    _config.connOpts = NULL;
    _config.client = NULL;
    _subscribe_object = NULL;
}
