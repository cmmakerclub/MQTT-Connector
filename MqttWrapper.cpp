#include "MqttWrapper.h"

MqttWrapper::MqttWrapper(const char* host, int port)
{

    prev_millis = millis();
    
    _mqtt_host = String(host);
    _mqtt_port = port;

    connOpts = NULL;
    client = NULL;

}

MqttWrapper::~MqttWrapper()
{
    delete connOpts;
    delete client;
    connOpts = NULL;
    client = NULL;
}
