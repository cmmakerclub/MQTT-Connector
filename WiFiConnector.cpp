#include "WiFiConnector.h"

WiFiConnector::WiFiConnector(const char* ssid, const char* password)
{
    init_config(ssid, password);

}

void WiFiConnector::init_config(const char* ssid, const char* password)
{
    prev_millis = millis();

    _ssid = String(ssid);
    _password = String(password);
}

void WiFiConnector::begin()
{
    _connect();
}
void WiFiConnector::connect()
{
    _connect();
}


void WiFiConnector::on_disconnected(wifi_callback_t callback)
{
    _user_on_disconnected = callback;
}

void WiFiConnector::on_connected(wifi_callback_t callback)
{
    _user_on_connected = callback;
}

void WiFiConnector::on_connecting(wifi_callback_t callback)
{
    _user_on_connecting = callback;
}

void WiFiConnector::loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        // return WiFi.status();
    }
    else
    {
        INFO_PRINTLN("CLASS: WIFI CONNECTED...");
        if (_user_on_disconnected != NULL)
        {
            _user_on_disconnected("wifi: WiFi disconnected.");
        }

        INFO_PRINTLN("RECONNECTING WIFI...");
        _connect();
    }
}

void WiFiConnector::_connect()
{
    _retries = 0;
    WiFi.begin(_ssid.c_str(), _password.c_str());

    while ((WiFi.status() != WL_CONNECTED))
    {
        if (_user_on_connecting != NULL)
        {
            char buf[3];
            itoa(WiFi.status(), buf, 10);
            _user_on_connecting((void*) buf);
        }

        DEBUG_PRINT(WiFi.status());
        DEBUG_PRINTLN(" ");

        INFO_PRINT(WiFi.status());
        INFO_PRINTLN(" ");
        _retries++;
        delay(500);
    }

    INFO_PRINTLN();
    INFO_PRINTLN("CLASS: WIFI CONNECTED...");

    if (_user_on_connected != NULL)
    {
        _user_on_connected("class: WIFI connected.");
    }

}


WiFiConnector::~WiFiConnector()
{

}
