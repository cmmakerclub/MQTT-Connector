MqttConnector::prepare_data_hook_t on_prepare_data =
[&](JsonObject * root) -> void {
    static float t_dht = 0;
    static float h_dht = 0;

    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    
    data["myName"] = DEVICE_NAME;
    data["millis"] = millis();
};
