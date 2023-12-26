#pragma once
#include <Arduino.h>
#include <main.h>
#include <HardwareSerial.h>
#include <WiFi.h>

#define RX_PIN 16
#define TX_PIN 17

HardwareSerial sim800(2);

esp_err_t sim800_init();
void receiveSMS(void *parameters);
static void updateSerial();
void sendSMS(const char *message);

esp_err_t sim800_init()
{
  Serial.print("Message configuration");
  sim800.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  sim800.println("AT");
  updateSerial();
  sim800.println("AT+CPIN?");
  updateSerial();
  sim800.println("AT+CFUN=1");
  updateSerial();
  sim800.println("AT+CMGF=1");
  updateSerial();
  sim800.println("AT+CSQ");
  updateSerial();
  sim800.println("AT+COPS?");
  updateSerial();
  sim800.println("AT+CREG?");
  updateSerial();
  sim800.println("AT+CCID");
  updateSerial();
  sim800.println("AT+CNMI=1,2,0,0,0");
  updateSerial();
  Serial.print("Message configuration end");
  Serial.println();
  return ESP_OK;
}

void sendSMS(const char *message)
{

  updateSerial();
  sim800.println("AT+CMGS=\"+541163218781\"");
  updateSerial();
  sim800.print(message);
  updateSerial();
  sim800.write(26);
  delay(500);
}

void receiveSMS(void *parameters)
{
  Serial.println("Esperando mensajes SMS...");

  while (true)
  {
    if (sim800.available())
    {
      String sms = sim800.readString();
      Serial.println("Mensaje recibido: " + sms);

      if (sms.indexOf("IP") >= 0)
      {
        updateSerial();
        Serial.print("Request IP");
        String ip = WiFi.localIP().toString();
        String message = "La direccion IP es: " + ip + "Y el CDN es: http://espupload.local";
        sendSMS(message.c_str());
      }
      if (sms.indexOf("Reset") >= 0)
      {
        updateSerial();
        ESP.restart();
      }
    }

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

static void updateSerial()
{
  delay(300);
  if (sim800.available())
  {
    Serial.print(sim800.read());
  }
  // if (Serial.available())
  // {
  //   sim800.write(Serial.read());
  // }
}