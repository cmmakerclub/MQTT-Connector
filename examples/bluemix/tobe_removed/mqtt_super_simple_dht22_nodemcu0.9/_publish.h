MqttConnector::prepare_data_hook_t on_prepare_data = 
[&](JsonObject * root) -> void {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    data["myName"] = DEVICE_NAME;
    static float t_dht;
    static float h_dht;

    read_dht(dht, &t_dht, &h_dht);

    data["temp"] = t_dht;
    data["humid"] = h_dht;
    
    info["author"] = AUTHOR;
    info["board"]  = BOARD;
    info["tag"]    = PROJECT;
    info["sensor"] = SENSOR;
};
