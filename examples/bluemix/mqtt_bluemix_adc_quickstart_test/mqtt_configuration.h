void hook_configuration(MqttWrapper::Config config)
{
    uint8_t mac[6];
    WiFi.macAddress(mac);

    // device id
    String macAddr;
    for (int i = 0; i < 6; ++i)
    {
        macAddr += String(mac[i], 16);
    }

    // d:quickstart:<type-id>:<device-id>
    *(config.clientId)  = String("d:quickstart:esp8266meetup:");
    *(config.clientId) += macAddr;

    *(config.topicPub)  = "iot-2/evt/status/fmt/json";
    
    String url  = String("https://quickstart.internetofthings.");
           url += "ibmcloud.com/#/device/"+ macAddr +"/sensor/";

    Serial.println(url);
}