#ifndef __MQTT_OTA_H
#define __MQTT_OTA_H
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#ifdef ESP8266
extern "C" {
  #include "user_interface.h"
}
#endif


bool rtcUserMemoryRead(uint32_t offset, uint32_t *data, size_t size)
{
    if (size + offset > 512) {
        return false;
    } else {
        return system_rtc_mem_read(64 + offset, data, size);
    }
}

bool rtcUserMemoryWrite(uint32_t offset, uint32_t *data, size_t size)
{
    if (size + offset > 512) {
        return false;
    } else {
        return system_rtc_mem_write(64 + offset, data, size);
    }
}

class MQTT_OTA
{
  private:
      struct {
        uint32_t crc32;
        byte data[508];
      } rtcData;
public:
  MQTT_OTA() {

  }
  void clear() {
    for (int i = 0; i < sizeof(rtcData); i++) {
      rtcData.data[i] = 0xFF;
    }
    rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData));
  }

  void write(const uint8_t *data, size_t length) {
    // Generate new data set for the struct
    uint8_t *pdata;
    rtcData.data[0] = 0x10;
    rtcData.data[1] = 0x0a;
    rtcData.data[2] = (uint8_t)length;

    pdata = const_cast<uint8_t*>(data);


    for (int i = 0; i < length; i++) {
      rtcData.data[i+3] = *pdata++;
    }

    rtcData.data[length+3] = 0xBB;

    // Update CRC32 of data
    rtcData.crc32 = calculateCRC32(((uint8_t*) &rtcData) + 4, sizeof(rtcData) - 4);
    // Write struct to RTC memory
    if (rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData))) {
      Serial.println("Write: ");
      printMemory();
      Serial.println();
    }
  }

  void read() {
    // Read struct from RTC memory
    if (rtcUserMemoryRead(0, (uint32_t*) &rtcData, sizeof(rtcData))) {
      Serial.println("Read: ");
      printMemory();
      Serial.println();
      uint32_t crcOfData = calculateCRC32(((uint8_t*) &rtcData) + 4, sizeof(rtcData) - 4);
      Serial.print("CRC32 of data: ");
      Serial.println(crcOfData, HEX);
      Serial.print("CRC32 read from RTC: ");
      Serial.println(rtcData.crc32, HEX);
      if (crcOfData != rtcData.crc32) {
        Serial.println("CRC32 in RTC memory doesn't match CRC32 of data. Data is probably invalid!");
      }
      else {
        Serial.println("CRC32 check ok, data is probably valid.");
        char test[100];
        memcpy(test, rtcData.data+3, (uint8_t)rtcData.data[2]);
        test[rtcData.data[2]] = '\0';
        Serial.printf("CHAR = %s\r\n", test);


        t_httpUpdate_return ret = ESPhttpUpdate.update(test);
        //t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");

        switch(ret) {
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("HTTP_UPDATE_NO_UPDATES");
                break;

            case HTTP_UPDATE_OK:
                Serial.println("HTTP_UPDATE_OK");
                break;
        }
      }
    }
  }
  uint32_t calculateCRC32(const uint8_t *data, size_t length)
  {
    uint32_t crc = 0xffffffff;
    while (length--) {
      uint8_t c = *data++;
      for (uint32_t i = 0x80; i > 0; i >>= 1) {
        bool bit = crc & 0x80000000;
        if (c & i) {
          bit = !bit;
        }
        crc <<= 1;
        if (bit) {
          crc ^= 0x04c11db7;
        }
      }
    }
    return crc;
  }

  void printMemory() {
    char buf[3];
    for (int i = 0; i < sizeof(rtcData); i++) {
      sprintf(buf, "%02X", rtcData.data[i]);
      Serial.print(buf);
      if ((i + 1) % 32 == 0) {
        Serial.println();
      }
      else {
        Serial.print(" ");
      }
    }
    Serial.println();
  }
};


#endif
