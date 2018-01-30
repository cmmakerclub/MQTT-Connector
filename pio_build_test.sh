
platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=espresso_lite_v2 "examples/basic_bme280"
platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=espresso_lite_v2 "examples/basic_mqtt"
platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=espresso_lite_v2 "examples/basic_dht"
