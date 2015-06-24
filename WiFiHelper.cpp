#include "WiFiHelper.h"

WiFiHelper::WiFiHelper(const char* ssid, const char* password)
{
    init_config(ssid, password);

}

void WiFiHelper::init_config(const char* ssid, const char* password)
{
    prev_millis = millis();

    _ssid = String(ssid);
    _password = String(password);

}


WiFiHelper::~WiFiHelper()
{

}
