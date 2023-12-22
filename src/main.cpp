#include <Arduino.h>
#include <wifi_task.h>
#include <ota.h>
#include <mqtt.h>
#include <can_utils.h>
#include <create_task.h>
#include <sim.h>

static void check_flags(esp_err_t process_flag);

void setup()
{
  Serial.begin(9600);
  check_flags(wifi_server_conection());
  check_flags(build_web_server());
  check_flags(mqtt_server_init());
  check_flags(sim800_init());
  check_flags(can_init());
  create_task();
}

void loop()
{
  loop_server();
  mqtt_loop();
  can_recibe_signal();
  delay(1000);
}

static void check_flags(esp_err_t process_flag)
{
  if (process_flag != ESP_OK)
  {
    ESP.restart();
  }
}