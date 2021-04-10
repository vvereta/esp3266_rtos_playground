
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "main";

struct Data {
    float temperture;
    int pressure;
};

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

void vTaskWriteData(void *pvParameters) {
    const char *pcTaskName = "Task 3 is running";
    struct Data* data = pvParameters;

    while(1) {
        ESP_LOGI(TAG, "%s", pcTaskName);
        data->pressure++;
        data->temperture += 0.5;
        vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

void vTaskPrintData(void *pvParameters) {
    const char *pcTaskName = "Task 4 is running";
    struct Data* data = pvParameters;

    while(1) {
        ESP_LOGI(TAG, "%s", pcTaskName);
        ESP_LOGI(TAG, "Temp is %d, pressure is %d", (int)data->temperture, data->pressure);
        vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

void app_main() {
    struct Data data = {42.42, 12};

    xTaskCreate(vTask1, "Task 1", 1024, NULL, 10, NULL );
    xTaskCreate(vTask2, "Task 2", 1024, NULL, 10, NULL );
    xTaskCreate(vTaskWriteData, "Task Write Data", 1024, &data, 10, NULL );
    xTaskCreate(vTaskPrintData, "Task Print Data", 1024, &data, 10, NULL );
}
