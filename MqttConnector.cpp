/*

Copyright Nat Weerawan 2015-2016
MIT License

The MIT License (MIT)

Copyright (c) Nat Weerawan <nat.wrw@gmail.com> (cmmakerclub.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

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
    String flashId = String(ESP.getFlashChipId(), HEX);
    String chipId = String(ESP.getChipId(), HEX);
    flashId.toUpperCase();
    chipId.toUpperCase();

    prev_millis = millis();

    _mqtt_host = String(host);
    _mqtt_port = port;

    _config.connOpts = NULL;
    client = NULL;
    
    _subscribe_object = NULL;
    _subscribe_object = new MQTT::Subscribe();

    _config.enableLastWill = true;
    _config.publishOnly = false;
    _config.subscribeOnly = false;
    _config.retainPublishMessage = false;

    JsonObject& r = jsonRootBuffer.createObject();
    JsonObject& info = r.createNestedObject("info");
    JsonObject& dd = jsonDBuffer.createObject();

    this->root = &r;
    this->info = &info;

    
    r["info"] = info;
    r["d"] = dd;
    // this->d = &((JsonObject)r["d"]);
    this->d = &dd;
    static struct station_config conf;
    wifi_station_get_config(&conf);
    const char* ssid = reinterpret_cast<const char*>(conf.ssid);

    info["ssid"] =  ssid;
    info["flash_size"] = ESP.getFlashChipSize();
    // info["flash_id"] = flashId.c_str();
    info["chip_id"] = chipId.c_str();;
    info["sdk"] = system_get_sdk_version();

}

MqttConnector::MqttConnector(const char* host, uint16_t port, cmmc_config_t config_hook)
{
    init_config(host, port);
    _user_hook_config = config_hook;
}

void MqttConnector::_clear_last_will() {
    MQTT_DEBUG_PRINTLN("__CLEAR LASTWILL");
    MQTT_DEBUG_PRINT("WILL TOPIC: ");
    MQTT_DEBUG_PRINTLN(_config.topicLastWill);

    String willText = String("ONLINE|") + String(_config.clientId) + "|" + (millis()/1000); 
    uint8_t* payload = (uint8_t*) willText.c_str();
    MQTT::Publish newpub(_config.topicLastWill, payload, willText.length());
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

void MqttConnector::on_published(PubSubClient::callback_t callback) {
    if (callback != NULL)
    {
        MQTT_DEBUG_PRINTLN("__USER REGISTER SUBSCRIPTION CALLBACK");
        _user_on_published = callback;
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
    }

    _config.topicSub = _config.channelPrefix + String("/") + _config.clientId + String("/command");
    _config.topicPub = _config.channelPrefix + String("/") + _config.clientId + String("/status");
    _config.topicLastWill = _config.channelPrefix + String("/") + _config.clientId + String("/online");


    (*info)["id"] = _config.clientId.c_str();;

    _config.mqttHost = _mqtt_host;
    _config.mqttPort = _mqtt_port;

    MQTT_DEBUG_PRINT("__PUBLICATION TOPIC -> ");

    MQTT_DEBUG_PRINTLN(_config.topicPub)

    MQTT_DEBUG_PRINT("__SUBSCRIPTION TOPIC -> ");

    MQTT_DEBUG_PRINTLN(_config.topicSub);

    _config.connOpts = new MQTT::Connect(_config.clientId);
    _config.client = new PubSubClient(wclient);
    client = _config.client;
  
    client->set_server(_mqtt_host, _mqtt_port);
    client->set_callback(_on_message_arrived);

    _config.username.trim();
    _config.password.trim();
    if(_config.username == "" ||  _config.password == "") {
        // NO-NEED TO SET AUTH;
        MQTT_DEBUG_PRINT("NO-AUTH Connection.");
    }
    else {
        _config.connOpts->set_auth(_config.username, _config.password);
    }


    if (_config.enableLastWill) {
        MQTT_DEBUG_PRINT("ENABLE LAST WILL: ");
        String willText = String("DEAD|") + String(_config.clientId) + "|" + (millis()/1000); 
        int qos = 1;
        int retain = true;
        (_config.connOpts)->set_will(_config.topicLastWill, willText, qos, retain);
        (_config.connOpts)->set_clean_session(false);
        (_config.connOpts)->set_keepalive(15);
        MQTT_DEBUG_PRINTLN(_config.topicLastWill);
    }

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

void MqttConnector::doPublish(bool force)
{
    static long counter = 0;

    if (force || _timer_expired(&publish_timer))
    {
        if (!client->connected()) return;
        _timer_set(&publish_timer, _publish_interval);

        _prepare_data_hook();

        (*d)["version"] = _version.c_str();                
        IPAddress ip = WiFi.localIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + 
                       '.' + String(ip[2]) + '.' + String(ip[3]);
        (*d)["heap"] = ESP.getFreeHeap();
        (*d)["ip"] = ipStr.c_str();
        (*d)["rssi"] = WiFi.RSSI();
        (*d)["counter"] = ++counter;
        (*d)["seconds"] = millis()/1000;
        (*d)["subscription"] = String(_subscription_counter).c_str();
        

        _after_prepare_data_hook();


        strcpy(jsonStrbuffer, "");
        root->printTo(jsonStrbuffer, sizeof(jsonStrbuffer));
        // dataPtr = jsonStrbuffer;
        prev_millis = millis();

        MQTT_DEBUG_PRINTLN("__DO PUBLISH ");
        MQTT_DEBUG_PRINT("______________ TOPIC: -->");
        MQTT_DEBUG_PRINTLN(_config.topicPub);
        MQTT_DEBUG_PRINT("______________ CONTENT: -->");
        MQTT_DEBUG_PRINTLN(jsonStrbuffer);

        // if (_user_hook_publish_data != NULL)
        // {
        //     _user_hook_publish_data(dataPtr);
        // }

        MQTT::Publish newpub(_config.topicPub, (uint8_t*)jsonStrbuffer, strlen(jsonStrbuffer));
        if (_config.retainPublishMessage) {
            newpub.set_retain(true) ;
        }
        if(!client->publish(newpub)) {
            MQTT_DEBUG_PRINTLN("PUBLISHED FAILED!");
            return;
        }
        else {
            MQTT_DEBUG_PRINTLN("PUBLISHED SUCCEEDED!");
            if (_user_on_published) {
                _user_on_published(newpub);
            }

        }

        MQTT_DEBUG_PRINTLN("====================================");
    }
}

void MqttConnector::_connect()
{
    client->set_max_retries(150);
    bool flag = true;

    uint16_t times = 0;


    while(!client->connect(*(_config.connOpts)) && flag)
    {
        MQTT_DEBUG_PRINTLN("KEEP CONNECTING...");
        if (_user_hook_connecting) {
            _user_hook_connecting(++times, &flag);
            yield();
        }
        else {
            delay(100);
        }
    }

    MQTT_DEBUG_PRINTLN("== Wrapper.connect(); CONNECT WITH OPTIONS = ");
    MQTT_DEBUG_PRINTLN("== Wrapper.connect(); CONNECT WITH OPTIONS = ");
    MQTT_DEBUG_PRINT("HOST: ");
    MQTT_DEBUG_PRINTLN(_mqtt_host);
    MQTT_DEBUG_PRINT("PORT: ");
    MQTT_DEBUG_PRINTLN(_mqtt_port);
    MQTT_DEBUG_PRINT("clientId: ");
    MQTT_DEBUG_PRINTLN(_config.clientId);

    MQTT_DEBUG_PRINT("USER: ");
    MQTT_DEBUG_PRINTLN(_config.username);
    MQTT_DEBUG_PRINT("PASS: ");
    MQTT_DEBUG_PRINTLN(_config.password);
    MQTT_DEBUG_PRINT("clientId: ");
    MQTT_DEBUG_PRINTLN(_config.clientId);

    MQTT_DEBUG_PRINT("lastWill: ");
    MQTT_DEBUG_PRINTLN(_config.enableLastWill);

    MQTT_DEBUG_PRINTLN("CONNECTED");
    MQTT_DEBUG_PRINTLN("====================================");
    MQTT_DEBUG_PRINTLN("====================================");

    if (_config.publishOnly == true) {
       // delete _user_hook_prepare_subscribe;
       _user_hook_prepare_subscribe = NULL;
    }

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
    
    if (_config.enableLastWill) {
        _clear_last_will();
    }

    doPublish(true);
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
