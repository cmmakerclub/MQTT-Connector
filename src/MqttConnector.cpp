/*

Copyright Nat Weerawan 2017-2020
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

// 192.168.111.111
#define IP_C_STR_SIZE 16
static char *c_ipStr;
static String ipStr;

static String toStringIp(uint32_t ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

MqttConnector::MqttConnector(const char* host, uint16_t port)
{
    c_ipStr = (char*)malloc(IP_C_STR_SIZE);
    ipStr = toStringIp(WiFi.localIP());
    strcpy(c_ipStr, ipStr.c_str());

    _on_message_arrived = [&](const MQTT::Publish& pub) {
      _msg_arrived_ms = millis();
      wclient.flush();
        if (_user_on_message_arrived) {
            _user_on_message_arrived(pub);
        }

        if (_user_on_after_message_arrived != NULL) {
            int prefix_len = _config.channelPrefix.length();
            String topic = pub.topic();
            String p_topic = topic.substring(prefix_len);
            const char *p = p_topic.c_str();
            int fc = 0;
            while(*p++ != '/') { fc++; }
            _user_on_after_message_arrived(p_topic, p_topic.substring(fc+1), pub.payload_string());
        }
    };

    init_config(host, port);

    MQTT_DEBUG_PRINTLN("----------- Wrapper CONSTRUCTOR ---------");
    MQTT_DEBUG_PRINT(_mqtt_host);
    MQTT_DEBUG_PRINT(" - ");
    MQTT_DEBUG_PRINTLN(_mqtt_port);
    MQTT_DEBUG_PRINTLN("---------- /Wrapper CONSTRUCTOR ---------");
}

void MqttConnector::init_config(const char* host, uint16_t port)
{

    prev_millis = millis();

    _mqtt_host = String(host);
    _mqtt_port = port;

    _config.connOpts = NULL;
    _config.client = NULL;
    _config.mode = MODE_BOTH;
    _subscribe_object = NULL;
    _config.enableLastWill = true;
    _config.retainPublishMessage = false;
    _config.firstCapChannel = false;

    JsonObject& r = jsonRootBuffer.createObject();
    JsonObject& info = r.createNestedObject("info");
    JsonObject& dd = jsonDBuffer.createObject();

    this->root = &r;
    this->info = &info;


    r["info"] = info;
    r["d"] = dd;
    // this->d = &((JsonObject)r["d"]);
    this->d = &dd;

    #ifdef ESP8266
        static struct station_config conf;
        wifi_station_get_config(&conf);
        const char* ssid = reinterpret_cast<const char*>(conf.ssid);
        info["ssid"] =  String(ssid);
        info["flash_id"] = String(ESP.getFlashChipId(), HEX);
        info["chip_id"] = String(ESP.getChipId(), HEX);
    #else
        // wifi_config_t conf;
        // esp_wifi_get_config(WIFI_IF_STA, &conf);
        wifi_config_t conf;
        esp_wifi_get_config(WIFI_IF_STA, &conf);
        info["ssid"] =  String(reinterpret_cast<const char*>(conf.sta.ssid));
        info["chip_id"] = WiFi.macAddress();
        // info["chip_id"] = String(ESP.getChipId(), HEX);
    #endif

    String mac = String(WiFi.macAddress());
    mac.toLowerCase();
    // info["flash_id"] = ESP.getEfuseMac

    // info["sdk"] = String(system_get_sdk_version());
    info["flash_size"] = ESP.getFlashChipSize();
    info["mac"] = mac;
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

    static String willText = String("{\"status\":1")
    + String(",\"id\":\"") + String(_config.clientId)
    + "\"}";
    MQTT::Publish newpub(_config.topicLastWill, (uint8_t*) willText.c_str(), willText.length());
    newpub.set_retain(true);
    _config.client->publish(newpub);

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

void MqttConnector::on_published(after_publish_hook_t callback) {
  on_after_publish(callback);
}

void MqttConnector::on_after_publish(after_publish_hook_t callback) {
    if (callback != NULL)
    {
        MQTT_DEBUG_PRINTLN("__USER REGISTER SUBSCRIPTION CALLBACK");
        _user_on_after_publish = callback;
    }
    else
    {
        MQTT_DEBUG_PRINTLN("__USER DOES NOT REGISTER SUBSCRIPTION CALLBACk");
    }
}

void MqttConnector::connect()
{
    MQTT_DEBUG_PRINTLN("BEGIN CMMC_MQTT_CONNECTOR");
    _set_default_client_id();
    _hook_config();
    MQTT_DEBUG_PRINTLN("_hook_after_config");
    _hook_after_config();
    MQTT_DEBUG_PRINTLN("_connect");
    _connect();
}

void MqttConnector::_hook_config()
{
    if (_user_hook_config != NULL)
    {
        MQTT_DEBUG_PRINTLN("OVERRIDE CONFIG IN _hook_config");
        _user_hook_config(&_config);
    }

    if (_config.clientId == "") {
      _config.clientId = WiFi.macAddress();
    }

    String commandChannel = "/$/command";
    String statusChannel = "/status";
    String lwtChannel = "/lwt";

    if (_config.firstCapChannel) {
      commandChannel = "/$/Command";
      statusChannel = "/Status";
      lwtChannel = "/Lwt";
    }

    MQTT_DEBUG_PRINT("TOPIC SUB = ");
    MQTT_DEBUG_PRINTLN(_config.topicSub);
    MQTT_DEBUG_PRINT("TOPIC PUB = ");
    MQTT_DEBUG_PRINTLN(_config.topicPub);
    _config.topicSub.trim();
    _config.topicPub.trim();

    // subscribe
    if (_config.topicSub.length() == 0) {
      _config.topicSub = String(_config.channelPrefix) +
        String(_config.clientId) + commandChannel;
    }

    // publish
    if (_config.topicPub.length() == 0) {
      _config.topicPub = String(_config.channelPrefix) +
      String(_config.clientId) + statusChannel;
    }

    MQTT_DEBUG_PRINT("TOPIC SUB = ");
    MQTT_DEBUG_PRINTLN(_config.topicSub);
    MQTT_DEBUG_PRINT("TOPIC PUB = ");
    MQTT_DEBUG_PRINTLN(_config.topicSub);

    _config.topicLastWill = String(_config.channelPrefix) +
      String(_config.clientId) + lwtChannel;

    (*info)["id"] = _config.clientId;
    (*info)["client_id"] = _config.clientId;
    (*info)["device_id"] = WiFi.macAddress();
    (*info)["prefix"] = _config.channelPrefix;
    (*info)["ip"] = c_ipStr;

    _config.mqttHost = _mqtt_host;
    _config.mqttPort = _mqtt_port;

    MQTT_DEBUG_PRINT("__PUBLICATION TOPIC -> ");

    #ifdef MQTT_DEBUG_LEVEL_VERBOSE
    MQTT_DEBUG_PRINTLN(_config.topicPub)
    #endif
    MQTT_DEBUG_PRINT("__SUBSCRIPTION TOPIC -> ");
    #ifdef MQTT_DEBUG_LEVEL_VERBOSE
    MQTT_DEBUG_PRINTLN(_config.topicSub);
    #endif

    _config.connOpts = new MQTT::Connect(_config.clientId);
    _config.client = new PubSubClient(wclient);

    _config.client->set_server(_mqtt_host, _mqtt_port);
    _config.client->set_callback(_on_message_arrived);
    _config.username.trim();
    _config.password.trim();

    // NO-NEED TO SET AUTH;
    if(_config.username == "" ||  _config.password == "") {
        MQTT_DEBUG_PRINT("NO-AUTH Connection.");
    }
    else {
        _config.connOpts->set_auth(_config.username, _config.password);
    }

    _config.connOpts->set_clean_session(true);

    if (_config.enableLastWill) {
        MQTT_DEBUG_PRINT("ENABLE LAST WILL: ");
        // String willText = String("DEAD|") + String(_config.clientId) + "|" + (millis());
        static String willText = String("{\"status\":0,\"id\": \"") + String(_config.clientId) + "\"}";
        int qos = 1;
        int retain = true;
        (_config.connOpts)->set_will(_config.topicLastWill, willText, qos, retain);
        (_config.connOpts)->set_keepalive(15);
        #ifdef MQTT_DEBUG_LEVEL_VERBOSE
        MQTT_DEBUG_PRINTLN(_config.topicLastWill);
        #endif
    }

}

void MqttConnector::_hook_after_config()
{
    if (_user_hook_after_config != NULL)
    {
        MQTT_DEBUG_PRINTLN("OVERRIDE CONFIG IN _hook_config");
        _user_hook_after_config(_config);
    }

    if (_user_on_prepare_data_once) {
      _user_on_prepare_data_once();
    }

    if (_user_on_before_message_arrived_once) {
      _user_on_before_message_arrived_once();
    }
}

void MqttConnector::sync_pub(String payload)
{
    MQTT_DEBUG_PRINT("SYNC PUB.... -> ");
    MQTT_DEBUG_PRINTLN(payload.c_str());

    MQTT::Publish newpub(_config.topicSub, (uint8_t*)payload.c_str(), payload.length());
    newpub.set_retain(true);
    _config.client->publish(newpub);
}

void MqttConnector::sync_advpub(String prefix, String topic, String payload, bool retain)
{
    //prefix is actually not yet usable
    if (prefix != ""){
      topic=String(_config.channelPrefix) + String("/") + String(_config.clientId)+topic;
    }

    MQTT_DEBUG_PRINT("SYNC PUB TO CHANNEL ");
    MQTT_DEBUG_PRINT(topic.c_str());
    MQTT_DEBUG_PRINT(".... -> ");
    MQTT_DEBUG_PRINTLN(payload.c_str());

    MQTT::Publish newpub(topic, (uint8_t*)payload.c_str(), payload.length());
    newpub.set_retain(retain);
    _config.client->publish(newpub);
}

void MqttConnector::loop()
{
    if (_config.client->connected())
    {
      _config.client->loop();
      if (_config.mode == MODE_SUB_ONLY) {
        MQTT_DEBUG_PRINTLN("SUBSCRIBE ONLY MODE..");
      }
      else {
          doPublish();
      }
    }
    else
    {
        MQTT_DEBUG_PRINTLN("MQTT DISCONNECTED..");
        MQTT_DEBUG_PRINTLN("MQTT RECONNECTING..");
        _connect();
    }
}

void MqttConnector::_set_default_client_id() {
        MQTT_DEBUG_PRINTLN("SET_DEFAULT_MQTT_CLIENT_ID");
        String result = WiFi.macAddress();
        result.toLowerCase();
        _config.clientId = result;

        // MQTT_DEBUG_PRINT("MAC ADDR: ");
        // MQTT_DEBUG_PRINT(result);
        // MQTT_DEBUG_PRINT("\n");

        _config.channelPrefix = "esp8266";
        MQTT_DEBUG_PRINTLN("[1] /* END SET_DEFAULT_MQTT_CLIENT_ID */");
        MQTT_DEBUG_PRINTLN("[2] /* END SET_DEFAULT_MQTT_CLIENT_ID */");

    }

void MqttConnector::doPublish(bool force)
{
    static long counter = 0;
    if (force || _timer_expired(&_publish_timer))
    {
        if (!_config.client->connected()) return;
        unsigned long __dif = (millis() - _msg_arrived_ms);
        // MQTT_DEBUG_PRINTf("DIFF == %lu \r\n", __dif);
        if (__dif <= 1000)  {
          // MQTT_DEBUG_PRINTLN("PUBLICATION SKIPPED.");
          return;
        }

        MQTT_DEBUG_PRINTLN("PUBLICATION PASSED.");

        _timer_set(&_publish_timer, _publish_interval);
        _prepare_data_hook();

        (*info)["version"] = _version;
        (*d)["heap"] = ESP.getFreeHeap();
        (*d)["rssi"] = WiFi.RSSI();
        (*d)["counter"] = ++counter;
        (*d)["millis"] = millis();
        (*d)["subscription"] = _subscription_counter;

        _after_prepare_data_hook();

        strcpy(jsonStrbuffer, "");
        root->printTo(jsonStrbuffer, sizeof(jsonStrbuffer));
        // dataPtr = jsonStrbuffer;
        prev_millis = millis();

        MQTT_DEBUG_PRINTLN("PUBLISH: ");
        MQTT_DEBUG_PRINT("______________ TOPIC: -->");
        MQTT_DEBUG_PRINT(_config.topicPub);
        MQTT_DEBUG_PRINTLN();

        MQTT_DEBUG_PRINT("______________ CONTENT: -->");
        #ifdef MQTT_DEBUG_LEVEL_VERBOSE
        MQTT_DEBUG_PRINT(jsonStrbuffer);
        #endif
        MQTT_DEBUG_PRINTLN();

        MQTT::Publish newpub(_config.topicPub, (uint8_t*)jsonStrbuffer, strlen(jsonStrbuffer));
        if (_config.retainPublishMessage) {
            newpub.set_retain(true) ;
        }

        if(!_config.client->publish(newpub)) {
            MQTT_DEBUG_PRINTLN();
            MQTT_DEBUG_PRINTLN("PUBLISHED FAILED!");
            return;
        }
        else {
            MQTT_DEBUG_PRINTLN();
            MQTT_DEBUG_PRINTLN("PUBLISHED SUCCEEDED!");
            if (_user_on_after_publish) {
                _user_on_after_publish(newpub);
            }
        }

        MQTT_DEBUG_PRINTLN("====================================");
        MQTT_DEBUG_PRINTLN("====================================");
    }
}

void MqttConnector::_connect()
{
    // _config.client->set_max_retries(150);
    bool flag = true;

    uint16_t times = 0;

    while(!_config.client->connect(*(_config.connOpts)) && flag)
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

    if (_config.mode == MODE_PUB_ONLY) {
       // delete _user_hook_subscribe;
       _user_hook_subscribe = NULL;
    }

    if (_subscribe_object != NULL) {
      delete _subscribe_object;
      _subscribe_object = NULL;
      _subscription_counter=0;
    }
    _subscribe_object = new MQTT::Subscribe();

    /*
    * BEGIN SUBSCRIBE LOGIC
    */
    if (_user_hook_subscribe != NULL)
    {
        MQTT_DEBUG_PRINTLN("CALLING HOOK SUBSCRIBING..");
        _user_hook_subscribe(_subscribe_object);
        MQTT_DEBUG_PRINTLN("CHECK IF __SUBSCRIBING... ->");
        _subscribe_object->add_topic(_config.topicSub);

        MQTT_DEBUG_PRINTLN("++TRY SUBSCRIBING ++");
            if (_config.client->subscribe(*_subscribe_object)) {
                _subscription_counter++;
                MQTT_DEBUG_PRINT("__SUBSCRIBED TOPICS ___");
                // MQTT_DEBUG_PRINTLN(_config.topicSub);
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

// HOOKS
// void MqttConnector::connecting_hook(int count, bool *flag) {
//     if (_user_hook_connecting != NULL) {
//         _user_hook_connecting(count, flag);
//     }
// }

void MqttConnector::on_prepare_configuration(cmmc_config_t func)
{
    _user_hook_config = func;
}

void MqttConnector::on_after_prepare_configuration(cmmc_after_config_t func)
{
    _user_hook_after_config = func;
}

void MqttConnector::on_connecting(connecting_hook_t cb) {
    _user_hook_connecting = cb;
}

void MqttConnector::on_prepare_data(prepare_data_hook_t func,
  unsigned long publish_interval)
{
    _user_hook_prepare_data = func;
    _publish_interval = publish_interval;
    _timer_set(&_publish_timer, publish_interval);

}

void MqttConnector::on_after_prepare_data(after_prepare_data_hook_t func)
{
    _user_hook_after_prepare_data = func;
}

void MqttConnector::on_subscribe(subscribe_hook_t func)
{
    _user_hook_subscribe = func;
}

void MqttConnector::_prepare_data_hook()
{
    if (_user_on_before_prepare_data != NULL) {
      _user_on_before_prepare_data();
    }

    if (_user_hook_prepare_data != NULL)
    {
        MQTT_DEBUG_PRINTLN("__user_hook_prepare_data()");
        _user_hook_prepare_data(root);

        if (_user_hook_after_prepare_data) {
            _user_hook_after_prepare_data(root);
        }

    }

}

void MqttConnector::_after_prepare_data_hook()
{
    if (_user_hook_after_prepare_data != NULL)
    {
        MQTT_DEBUG_PRINTLN("__user_hook_after_prepare_data()");
        _user_hook_after_prepare_data(root);
    }
    // MQTT_DEBUG_PRINTLN("BEFORE PUBLISH");
}


void MqttConnector::_hook_after_publish(char** ptr)
{
    // MQTT_DEBUG_PRINTLN("AFTER PUBLISH");
}

void MqttConnector::on_prepare_data_once(before_prepare_data_once_t func) {
    _user_on_prepare_data_once = func;
}

void MqttConnector::on_before_prepare_data(before_prepare_data_hook_t func) {
  _user_on_before_prepare_data = func;
}


MqttConnector::~MqttConnector()
{
    delete _config.connOpts;
    delete _config.client;
    delete _subscribe_object;

    _config.connOpts = NULL;
    _config.client = NULL;
    _subscribe_object = NULL;

    free(c_ipStr);
}
