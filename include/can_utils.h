#pragma once
#include <Arduino.h>
#include <CAN.h>
#include <mqtt.h>

#define TX_GIPO_NUM 21
#define RX_GIPO_NUM 22

esp_err_t can_init();
static void can_recibe_signal();

esp_err_t can_init()
{
  CAN.setPins(RX_GIPO_NUM, TX_GIPO_NUM);
  if (!CAN.begin(500E3))
  {
    return ESP_ERR_INVALID_STATE;
  }
  return ESP_OK;
}

static void can_recibe_signal()
{
  int packetSize = CAN.parsePacket();
  if (packetSize || CAN.packetId() == 0x12)
  {
    if (!CAN.packetRtr())
    {
      int attempt = 0;
      while (CAN.available() && attempt < MAX_TRYS)
      {
        ++attempt;
        Serial.print("Received ");
        Serial.println();
        int temperature_recived = CAN.read();
        if (temperature_recived >= 40 && temperature_recived <= 257)
        {
          int new_temperature = temperature_recived - 256;
          mqtt_send_message("freeze/temperature", (const char *)new_temperature);
          Serial.println();
        }
        else
        {
          mqtt_send_message("freeze/temperature", (const char *)temperature_recived);
          Serial.println();
        }
        delay(1000);
      }
    }
  }
}
