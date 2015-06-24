#include "MqttWrapper.h"

MqttWrapper::MqttWrapper(const char* host, uint16_t port)
{
    init_config(host, port);
    DEBUG_PRINTLN("----------- Wrapper CONSTRUCTOR ---------");
    DEBUG_PRINT(_mqtt_host);
    DEBUG_PRINT(" - ");
    DEBUG_PRINT(_mqtt_port);
    DEBUG_PRINTLN("---------- /Wrapper CONSTRUCTOR ---------");
}

void MqttWrapper::init_config(const char* host, uint16_t port)
{
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

MqttWrapper::MqttWrapper(const char* host, uint16_t port, cmmc_config_t config_hook)
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
