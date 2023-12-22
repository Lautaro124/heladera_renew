#pragma once
#include <Arduino.h>
#include <sim.h>
#include <main.h>

esp_err_t create_task()
{
  static uint8_t ucParameterToPass;
  TaskHandle_t xHandle = NULL;

  xTaskCreatePinnedToCore(
      receiveSMS,
      "Sim recibe",
      STACK_SIZE,
      &ucParameterToPass,
      1,
      &xHandle,
      1);

  return ESP_OK;
}