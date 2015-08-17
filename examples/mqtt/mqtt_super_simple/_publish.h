MqttConnector::prepare_data_hook_t on_prepare_data = 
[&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    data["myName"] = DEVICE_NAME;
    data["author"] = AUTHOR;
    data["board"]  = BOARD;
    data["tag"]    = PROJECT;
    data["sensor"] = SENSOR;
    data["value"]  = "YOUR_SENSOR_DATA";
};