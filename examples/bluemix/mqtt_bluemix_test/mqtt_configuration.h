void hook_configuration(MqttWrapper::Config config)
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    String macAddr;
    for (int i = 0; i < 6; ++i)
    {
        macAddr += String(mac[i], 16);
    }

    // a:org_id:device_id
    *(config.clientId) = String("a:r6crrd:") + macAddr;

    // API-KEY
    *(config.username) = String("a-r6crrd-3gsgqcadid");
    *(config.password) = String("KRE*0rxt0rilV!2tD@");

    char buf[100];
    const char* device_type = "esp8266";
    const char* event_id = "dw.mini";

    sprintf(buf, "iot-2/type/%s/id/%s/evt/%s/fmt/json", device_type, macAddr.c_str(), event_id);

    *(config.topicPub) = String(buf);

    Serial.println(".");
    Serial.println(macAddr);
}