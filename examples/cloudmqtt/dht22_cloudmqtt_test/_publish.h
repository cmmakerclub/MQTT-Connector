MqttConnector::prepare_data_hook_t on_prepare_data =
[&](JsonObject * root) -> void {
    float t_dht = dht.readTemperature();
    float h_dht = dht.readHumidity();

    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    data["myName"] = DEVICE_NAME;

    data["temp"] = t_dht;
    data["humid"] = h_dht;
};
