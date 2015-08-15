MqttConnector::prepare_data_hook_t on_prepare_data = 
[&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    data["myName"] = "NAzT";
    data["adc"] = analogRead(A0);
    data["tag"] = "paris";
    data["zone"] = "1";
};