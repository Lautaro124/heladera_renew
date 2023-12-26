#include <Arduino.h>
#include <wifi_task.h>
#include <ota.h>
#include <mqtt.h>
#include <can_utils.h>
#include <create_task.h>
#include <sim.h>
#include <esp_task_wdt.h>

const long interval_seconds = 1000;
unsigned long timer = 0;
static void check_flags(esp_err_t process_flag, const char *error_text, const char *success_text);

void setup()
{
  Serial.begin(9600);
  create_task();
  check_flags(sim800_init(), "Sim 800 no iniciado", "Sim 800 configurado");
  delay(1000);
  check_flags(can_init(), "Can desconectado", "Can conectado!");
  check_flags(wifi_server_conection(), "Wifi server no funciono", "Wifi server conectado");
  check_flags(build_web_server(), "Build web server no funciono", "Build web server conectado");
  check_flags(mqtt_server_init(), "Mqtt server no funciono", "Mqtt server conectado");
  esp_task_wdt_init(WDTO_8S, true);
  esp_task_wdt_add(NULL);
}

void loop()
{
  esp_task_wdt_reset();
  unsigned long current_timer = millis();
  if (current_timer - timer >= interval_seconds)
  {
    mqtt_loop();
  }
  loop_server();
  can_recibe_signal();
  delay(500);
}

static void check_flags(esp_err_t process_flag, const char *error_text, const char *success_text)
{
  if (process_flag != ESP_OK)
  {
    sendSMS(error_text);
    Serial.print(error_text);
    ESP.restart();
  }
  Serial.print(success_text);
  sendSMS(success_text);
}