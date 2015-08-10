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
        if (_user_on_disconnected != NULL)
        {
            _user_on_disconnected("wifi: WiFi disconnected.");
        }

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
            char buf[20];
            if (WiFi.status() == WL_CONNECT_FAILED) {
                strcpy(buf, "(4) WL_CONNECT_FAILED");
            }
            else if (WiFi.status() == WL_NO_SSID_AVAIL) {
                strcpy(buf, "(1) WL_NO_SSID_AVAIL");
            }
            else if (WiFi.status() == WL_DISCONNECTED) {
                strcpy(buf, "(6) WL_DISCONNECTED");
            }
            else {
                strcpy(buf, "CODE: !");
                itoa(WiFi.status(), buf+6, 10);
                buf[7] = '\0';
            }
            _user_on_connecting((void*) buf);
        }

        WIFI_DEBUG_PRINT(WiFi.status());
        WIFI_DEBUG_PRINTLN(" ");
        _retries++;
        yield();
    }


    if (_user_on_connected != NULL)
    {
        _user_on_connected("class: WIFI connected.");
    }

}


WiFiConnector::~WiFiConnector()
{

}
