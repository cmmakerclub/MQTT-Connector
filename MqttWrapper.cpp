#include "MqttWrapper.h"

MqttWrapper::MqttWrapper(const char* host, int port)
{
    init_config(host, port);
    Serial.println("----------- Wrapper CONSTRUCTOR ---------");
    Serial.println(_mqtt_host);
    Serial.println(_mqtt_port);
    Serial.println("---------- /Wrapper CONSTRUCTOR ---------");
}

void MqttWrapper::init_config(const char* host, int port) {
    prev_millis = millis();

    _mqtt_host = String(host);
    _mqtt_port = port;

    connOpts = NULL;
    client = NULL;

    JsonObject& r = jsonBuffer.createObject();
    JsonObject& d = jsonBuffer.createObject();

    this->root = &r;
    this->d = &d;

    r["d"] = d;
}

MqttWrapper::MqttWrapper(const char* host, int port, cmmc_config_t config_hook)
{
    init_config(host, port);
    _user_hook_config = config_hook;

}


MqttWrapper::~MqttWrapper()
{
    delete connOpts;
    delete client;
    connOpts = NULL;
    client = NULL;
}
