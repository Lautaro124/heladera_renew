#pragma once
#include <ElegantOTA.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>

WebServer server(80);

esp_err_t build_web_server();
esp_err_t configure_dns();
static void loop_server();

esp_err_t build_web_server()
{
  ElegantOTA.begin(&server);
  ElegantOTA.setAuth("Chucho", "Administrator");

  server.begin();
  if (!configure_dns())
  {
    Serial.println();
    Serial.print("Error creating dns server");
  }

  MDNS.addService("http", "tcp", 80);
  Serial.println();
  Serial.println("HTTP server started");
  Serial.println();
  return ESP_OK;
}

esp_err_t configure_dns()
{
  if (MDNS.begin("espupload"))
  {
    return ESP_ERR_ESP_NETIF_DNS_NOT_CONFIGURED;
  }
  return ESP_OK;
}

static void loop_server()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    ElegantOTA.loop();
    server.handleClient();
  }
  else
  {
    Serial.print("Error connecting");
  }
}