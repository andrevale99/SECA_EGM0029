#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/uart.h>
#include <driver/gpio.h>

#include <esp_err.h>
#include <esp_log.h>

#include "ModBus.h"

#define RX_PIN GPIO_NUM_3
#define TX_PIN GPIO_NUM_1
#define BAUD_RATE_UART 115200
#define UART_PORT UART_NUM_0
#define BUF_SIZE 1024

void vTaskUART(void *pvArgs);

void app_main(void)
{
    MODBUS_config(RX_PIN, TX_PIN, BAUD_RATE_UART, UART_PORT, BUF_SIZE);
}