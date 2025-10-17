#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/uart.h>
#include <driver/gpio.h>

#include <esp_err.h>
#include <esp_log.h>

#define RX_PIN GPIO_NUM_3
#define TX_PIN GPIO_NUM_1

#define BAUD_RATE_UART 115200

#define UART_PORT UART_NUM_0

#define BUF_SIZE 1024

void vTaskUART(void *pvArgs);

esp_err_t UART_config(void);

void app_main(void)
{
    UART_config();

    xTaskCreate(vTaskUART, "TASK UART", 2048, NULL, 1, NULL);
}

void vTaskUART(void *pvArgs)
{
    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
    uint16_t idx = 0;
    while (1)
    {
        // Read data from the UART
        int len = uart_read_bytes(UART_PORT, &data[idx], 1, pdMS_TO_TICKS(20));
        // Write data back to the UART
        uart_write_bytes(UART_PORT, &data[idx], len);
        if (len)
        { 
            
            idx++;
            if (data[idx - 1] == '\n' || data[idx - 1] == '\r')
            {
                data[idx] = '\0';
                ESP_LOGI("[UART_TASK]", "Recv str: %s", (char *)data);

                idx = 0;
            }
        }
    }
}

esp_err_t UART_config(void)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE_UART,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, TX_PIN, RX_PIN, -1, -1));

    return ESP_OK;
}