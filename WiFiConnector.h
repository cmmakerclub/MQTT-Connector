#ifndef WIFI_CONNECTOR_H
#define WIFI_CONNECTOR_H

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

// TYPES
// typedef enum {
//     WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
//     WL_IDLE_STATUS      = 0,
//     WL_NO_SSID_AVAIL    = 1,
//     WL_SCAN_COMPLETED   = 2,
//     WL_CONNECTED        = 3,
//     WL_CONNECT_FAILED   = 4,
//     WL_CONNECTION_LOST  = 5,
//     WL_DISCONNECTED     = 6
// } wl_status_t;


class WiFiConnector
{
public:
    typedef std::function<void(const void*)> wifi_callback_t;

    WiFiConnector(const char* ssid, const char* password);
    ~WiFiConnector();

    void init_config(const char*, const char*);


    void connect();
    void begin();
    void loop();

    // CALLBACKS
    void on_disconnected(wifi_callback_t callback = NULL);
    void on_connected(wifi_callback_t callback = NULL);
    void on_connecting(wifi_callback_t callback = NULL);

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


    void _connect();
};



#endif//WIFI_CONNECTOR_H
