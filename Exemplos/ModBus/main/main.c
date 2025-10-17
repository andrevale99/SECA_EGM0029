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

typedef struct ModBusData
{
    uint8_t addr;
    uint8_t func;
} ModBusData_t;

void vTaskUART(void *pvArgs);

esp_err_t UART_config(void);

void decodeModBus(ModBusData_t *modbusLine, uint8_t *data);

void app_main(void)
{
    UART_config();

    xTaskCreate(vTaskUART, "TASK UART", 2048, NULL, 1, NULL);
}

void vTaskUART(void *pvArgs)
{
    const char *TAG = "[UART_TASK]";
    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
    uint16_t idx = 0;

    ModBusData_t modbusLine = {
        .addr = 0,
        .func = 0,
    };

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
                ESP_LOGI(TAG, "Recv str: %s", (char *)data);

                idx = 0;

                decodeModBus(&modbusLine, data);

                ESP_LOGI(TAG, "addr: %X", modbusLine.addr);
                ESP_LOGI(TAG, "func: %X", modbusLine.func);
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

void decodeModBus(ModBusData_t *modbusLine, uint8_t *data)
{

    modbusLine->addr = (data[0] <= '9') ? ((data[0] - '0') << 4) : ((data[0] - 55) << 4);
    modbusLine->addr |= ((data[1] <= '9') ? ((data[1] - '0') & 0x0F) : ((data[1] - 55) & 0x0F));

    modbusLine->func = (data[2] <= '9') ? ((data[2] - '0') << 4) : ((data[2] - 55) << 4);
    modbusLine->func |= ((data[3] <= '9') ? ((data[3] - '0') & 0x0F) : ((data[3] - 55) & 0x0F));
}