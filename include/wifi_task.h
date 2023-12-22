#pragma once
#include <WiFi.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>
#include <main.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

esp_err_t wifi_server_conection();
esp_err_t wifi_conect(const char *ssid, const char *password);
esp_err_t read_credentials_saved();
static void save_credentials(const char *ssid, const char *password);

esp_err_t wifi_server_conection()
{
  WiFi.mode(WIFI_STA);
  WiFiManager wiFiManager;

  if (read_credentials_saved() != ESP_OK)
  {
    wiFiManager.autoConnect("ESP32-AP");
    save_credentials(wiFiManager.getWiFiSSID().c_str(), wiFiManager.getWiFiPass().c_str());
  }
  Serial.print("Conecctrion successfully");
  return ESP_OK;
}

static void save_credentials(const char *ssid, const char *password)
{
  DynamicJsonDocument jsonDoc(1024);
  jsonDoc["ssid"] = ssid;
  jsonDoc["password"] = password;

  File configFile = SPIFFS.open("/config.json", "w");
  if (configFile)
  {
    serializeJson(jsonDoc, configFile);
    configFile.close();
  }
}

esp_err_t read_credentials_saved()
{
  File configFile = SPIFFS.open("/config.json", "r");
  if (configFile)
  {
    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    configFile.close();

    DynamicJsonDocument jsonDoc(1024);
    DeserializationError error = deserializeJson(jsonDoc, buf.get());
    if (!error)
    {
      const char *saved_ssid = jsonDoc["ssid"].as<const char *>();
      const char *saved_password = jsonDoc["password"].as<const char *>();
      if (strlen(saved_ssid) > 0 && strlen(saved_password) > 0)
      {
        return wifi_conect(saved_ssid, saved_password);
      }
    }
  }
  return ESP_ERR_ESP_NETIF_BASE;
}

esp_err_t wifi_conect(const char *ssid, const char *password)
{
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < MAX_TRYS)
  {
    delay(500);
    ++attempts;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    return ESP_OK;
  }
  return ESP_ERR_WIFI_CONN;
}