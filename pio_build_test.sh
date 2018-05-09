
pip install --user platformio
platformio lib -g install 31 64 166 19 31
BOARD=espresso_lite_v2

rm -r test

for EXAMPLE in $PWD/examples/*; 
do
	platformio ci --project-option="lib_ldf_mode=2" $EXAMPLE -l '.' -b $BOARD
done

# platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=espresso_lite_v2 "examples/basic_bme280"
# platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=espresso_lite_v2 "examples/basic_mqtt"
# platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=espresso_lite_v2 "examples/basic_dht"
