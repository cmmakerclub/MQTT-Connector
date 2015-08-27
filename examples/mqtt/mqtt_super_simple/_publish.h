MqttConnector::prepare_data_hook_t on_prepare_data = 
[&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    JsonObject& info = root->at("info");
    data["myName"] = DEVICE_NAME;
    data["value"]  = analogRead(A0);

    info["author"] = AUTHOR;
    info["board"]  = BOARD;
    info["tag"]    = PROJECT;
    info["sensor"] = SENSOR;
};