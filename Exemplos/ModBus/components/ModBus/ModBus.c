#include <stdio.h>
#include "ModBus.h"

static uart_port_t uartPortGlobal;
static int bufferSize = 0;

esp_err_t MODBUS_config(int RXpin, int TXpin, int baudRate, uart_port_t uartPort, int bufSize)
{
    uartPortGlobal = uartPort;
    bufferSize = bufSize;
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = baudRate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(uartPort, bufSize * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(uartPort, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uartPort, TXpin, RXpin, -1, -1));

    xTaskCreate(VTaskModBus, "TASK UART", 2048, NULL, 1, NULL);

    return ESP_OK;
}

void decodeModBus(ModBusData_t *modbusLine, uint8_t *data)
{

    modbusLine->addr = (data[0] <= '9') ? ((data[0] - '0') << 4) : ((data[0] - 55) << 4);
    modbusLine->addr |= ((data[1] <= '9') ? ((data[1] - '0') & 0x0F) : ((data[1] - 55) & 0x0F));

    modbusLine->func = (data[2] <= '9') ? ((data[2] - '0') << 4) : ((data[2] - 55) << 4);
    modbusLine->func |= ((data[3] <= '9') ? ((data[3] - '0') & 0x0F) : ((data[3] - 55) & 0x0F));
}

uint16_t crc16_ccitt(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFF; // common init (variant: 0x0000 or 0x1D0F exist)
    const uint16_t poly = 0x8005;
    for (size_t i = 0; i < len; ++i)
    {
        crc ^= (uint16_t)data[i] << 8;
        for (int b = 0; b < 8; ++b)
        {
            if (crc & 0x8000)
                crc = (uint16_t)((crc << 1) ^ poly);
            else
                crc <<= 1;
        }
    }
    return crc;
}

void VTaskModBus(void *pvArgs)
{
    const char *TAG = "[UART_TASK]";
    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *)malloc(bufferSize);
    uint16_t idx = 0;

    ModBusData_t modbusLine = {
        .addr = 0,
        .func = 0,
    };

    while (1)
    {
        // Read data from the UART
        int len = uart_read_bytes(uartPortGlobal, &data[idx], 1, pdMS_TO_TICKS(20));
        // Write data back to the UART
        uart_write_bytes(uartPortGlobal, &data[idx], len);
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