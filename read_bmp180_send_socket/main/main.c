#include <string.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include <bmp180.h>
#include "connect.h"

#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#define PORT CONFIG_EXAMPLE_PORT

#define SDA_GPIO 4
#define SCL_GPIO 5

static const char *TAG = "tcp_example";

struct Data {
    uint32_t pressure;
    uint32_t temperature;

};

static void tcp_client_task(void *pvParameters)
{
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    while (1) {
        struct sockaddr_in destAddr;
        destAddr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);

        int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = connect(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            close(sock);
            continue;
        }
        ESP_LOGI(TAG, "Successfully connected");

        while (1) {
            int err = send(sock, pvParameters, sizeof(struct Data), 0);
            if (err < 0) {
                ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
                break;
            }

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

void read_bmp180(void *pvParameters)
{
    struct Data* data = pvParameters;
    bmp180_dev_t dev;
    memset(&dev, 0, sizeof(bmp180_dev_t)); // Zero descriptor

    ESP_ERROR_CHECK(bmp180_init_desc(&dev, 0, SDA_GPIO, SCL_GPIO));
    ESP_ERROR_CHECK(bmp180_init(&dev));

    while (1)
    {
        float temp;
        uint32_t pressure;

        esp_err_t res = bmp180_measure(&dev, &temp, &pressure, BMP180_MODE_STANDARD);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Could not measure: %d\n", res);
        } else {
                data->temperature = (uint32_t)temp;
                data->pressure = pressure;
            }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    // Socket routine
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    // i2c routine
    ESP_ERROR_CHECK(i2cdev_init());

    struct Data data = {};

    xTaskCreate(read_bmp180,     "read_bmp180", configMINIMAL_STACK_SIZE * 15, &data, 5, NULL);
    xTaskCreate(tcp_client_task, "tcp_client",  4096,                          &data, 5, NULL);
}
