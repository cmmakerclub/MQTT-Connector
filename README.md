# CMMC MQTT Connector [![Build Status](https://travis-ci.org/cmmakerclub/MQTT-Connector.svg?branch=master)](https://travis-ci.org/cmmakerclub/MQTT-Connector)
an esp8266 mqtt wrapper for chiang mai maker club

## Dependencies (provided)

 - https://github.com/bblanchon/ArduinoJson
 - http://github.com/Imroy/pubsubclient

## Arduino IDE configuration
To allow building an application using this library, you need to
"git clone" the project from within your Arduino library directory.  
On Windows 10, it should be under:  
`C:\Users\<your username>\Documents\Arduino\libraries\`  
Now you should be able to compile the examples.

## Usage
This library was initially intented to periodically communicate sensor data.

Provided examples use 3 files:

 - main arduino file	: define your connection details and the usual setup() loop() Arduino stuffs.
 - init_mqtt.h	: specify some protocol settings, subscriptions and implement you own connection event handlers 
 - _publish.h	: implementation of your publish hooks (periodic procedures relative to **outgoing** data: sensor read, data formatting...)  Data is sent to a default topic.
 - _receive.h	: implemention of your subscribe hooks (message handlers)  
 

@DoNcK also added the option to send data **spontaneously**, not periodically, on a specified topic, using:  
```void sync_advpub(String prefix, String topic, String payload, bool retain);```
(The *prefix* and *topic* arguments are not yet fully functional) 

**Warning:**  
It **not** asynchronous, so it will only return afer sending the payload. It can take some time on very loaded networks and prevent your main loop() from running smoothly.
