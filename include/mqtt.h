#pragma once
#include <PubSubClient.h>
#include <WiFi.h>
#include <main.h>

const char *mqtt_server = "198.199.86.120";
String clientId = "ESP32Client-";
const char *topicStatus = "freeze/espStatus";
WiFiClient espClient;
PubSubClient client(espClient);

esp_err_t mqtt_server_init();
esp_err_t mqtt_reconection();
esp_err_t mqtt_send_message(const char *topic, const char *payload);
void callback(char *topic, byte *payload, unsigned int length);
static void mqtt_loop();

esp_err_t mqtt_server_init()
{
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  if (!client.connected())
  {
    return ESP_OK;
  }
  return ESP_ERR_ESP_NETIF_INIT_FAILED;
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, topicStatus) == 0 && strcmp((char *)payload, "status") == 0)
  {
    mqtt_send_message(topicStatus, "true");
  }
}

esp_err_t mqtt_send_message(const char *topic, const char *payload)
{
  if (client.connect(clientId.c_str()))
  {
    if (client.publish(topic, payload))
    {
      return ESP_OK;
    }
  }
  return ESP_ERR_FLASH_NO_RESPONSE;
}

esp_err_t mqtt_reconection()
{
  int attempts = 0;
  while (!client.connected() && attempts < MAX_TRYS)
  {
    ++attempts;
    Serial.print("Attempting MQTT connection...");
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      client.publish(topicStatus, "true");
      return ESP_OK;
    }
  }
  return ESP_ERR_FLASH_NO_RESPONSE;
}

static void mqtt_loop()
{
  if (!client.connected())
  {
    mqtt_reconection();
  }
  client.loop();
  client.publish(topicStatus, "true");
}