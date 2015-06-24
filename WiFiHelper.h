#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

#include "ESP8266WiFi.h"
#include <functional>



#ifdef DEBUG_MODE
#ifndef DEBUG_PRINTER
#define DEBUG_PRINTER Serial
#endif
#ifdef DEBUG_LEVEL_VERBOSE
#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }

#define INFO_PRINT(...) { }
#define INFO_PRINTLN(...) { }
#else
#define DEBUG_LEVEL_INFO
#endif

#ifdef DEBUG_LEVEL_INFO
#define DEBUG_PRINT(...) {}
#define DEBUG_PRINTLN(...) {}

#define INFO_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define INFO_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#endif
#else
#define DEBUG_PRINT(...) {}
#define DEBUG_PRINTLN(...) {}

#define INFO_PRINT(...) { }
#define INFO_PRINTLN(...) { }
#endif




class WiFiHelper
{
public:
    typedef std::function<void(const char*)> wifi_callback_t;

    WiFiHelper(const char* ssid, const char* password);
    ~WiFiHelper();

    void init_config(const char*, const char*);


    void begin()
    {
        _connect();
    }

    void loop()
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
                _user_on_disconnected("class: disconnected");
            }
            _connect();
        }
    }

    void on_disconnected(wifi_callback_t callback = NULL)
    {
        _user_on_disconnected = callback;
    }

    void on_connected(wifi_callback_t callback = NULL)
    {
        _user_on_connected = callback;
    }

    void on_connecting(wifi_callback_t callback = NULL)
    {
        _user_on_connecting = callback;
    }

protected:

private:
    String _ssid = "";
    String _password = "";
    String _mac = "";

    wifi_callback_t _user_on_disconnected = NULL;
    wifi_callback_t _user_on_connected  = NULL;
    wifi_callback_t _user_on_connecting = NULL;

    unsigned long _retries = 0;
    unsigned long prev_millis;


    void _connect()
    {
        _retries = 0;
        WiFi.begin(_ssid.c_str(), _password.c_str());

        while ((WiFi.status() != WL_CONNECTED))
        {
            if (_user_on_connecting != NULL)
            {
                _user_on_connecting("class: connecting");
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
            _user_on_connected("class: connected");
        }

    }
};



#endif//WIFI_HELPER_H
