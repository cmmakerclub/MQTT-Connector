#include "MqttWrapper.h"

MqttWrapper::MqttWrapper(const char* host, int port)
{

    prev_millis = millis();

    _mqtt_host = String(host);
    _mqtt_port = port;

    connOpts = NULL;
    client = NULL;


    Serial.println("----------- Wrapper CONSTRUCTOR ---------");
    Serial.println(_mqtt_host);
    Serial.println(_mqtt_port);
    Serial.println("---------- /Wrapper CONSTRUCTOR ---------");
}


MqttWrapper::MqttWrapper(const char* host, int port, cmmc_config_t config_hook)
{
    prev_millis = millis();

    _mqtt_host = String(host);
    _mqtt_port = port;

    connOpts = NULL;
    client = NULL;


    _user_hook_config = config_hook;
}


MqttWrapper::~MqttWrapper()
{
    delete connOpts;
    delete client;
    connOpts = NULL;
    client = NULL;
}
