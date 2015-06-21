#include "MqttWrapper.h"

MqttWrapper::MqttWrapper(const char* host, int port)
{
    initConfig(host, port);
    Serial.println("----------- Wrapper CONSTRUCTOR ---------");
    Serial.println(_mqtt_host);
    Serial.println(_mqtt_port);
    Serial.println("---------- /Wrapper CONSTRUCTOR ---------");
}

void MqttWrapper::initConfig(const char* host, int port) {
    prev_millis = millis();

    _mqtt_host = String(host);
    _mqtt_port = port;

    connOpts = NULL;
    client = NULL;
}

MqttWrapper::MqttWrapper(const char* host, int port, cmmc_config_t config_hook)
{
    initConfig(host, port);
    _user_hook_config = config_hook;
}


MqttWrapper::~MqttWrapper()
{
    delete connOpts;
    delete client;
    connOpts = NULL;
    client = NULL;
}
