MqttConnector::prepare_data_hook_t on_prepare_data = 
[&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    data["myName"] = "TONG";
    data["tag"] = "cmmc";
    data["zone"] = "1";
};