
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "main";

void vTask1(void *pvParameters) {
    const char *pcTaskName = "Task 1 is running";

    while(1) {
        ESP_LOGI(TAG, "%s", pcTaskName);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void vTask2(void *pvParameters) {
    const char *pcTaskName = "Task 2 is running";

    while(1) {
        ESP_LOGI(TAG, "%s", pcTaskName);
        vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

void app_main() {
    xTaskCreate(vTask1, "Task 1", 1024, NULL, 10, NULL );
    xTaskCreate(vTask2, "Task 2", 1024, NULL, 10, NULL );
}
