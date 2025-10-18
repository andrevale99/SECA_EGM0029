#ifndef MODBUS_H
#define MODBUS_H

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_err.h>
#include <driver/uart.h>
#include <driver/gpio.h>
#include <esp_check.h>

typedef struct ModBusData
{
    uint8_t addr;
    uint8_t func;
    uint8_t data[256];
} ModBusData_t;

/// @brief Configuração do Modibus Utilizando o protocolo UART
/// @param RXpin Pino RX
/// @param TXpin Pino TX
/// @param baudRate Baud Rate da comunicação
/// @param uartPort Porta UART a ser utilizada
/// @param bufSize Tamanho do buffer de recepção
/// @return ESP_OK se a configuração for bem sucedida, 
/// caso contrário retorna um código de erro.
esp_err_t MODBUS_config(int RXpin, int TXpin, int baudRate,
                      uart_port_t uartPort, int bufSize);

/// @brief Decodifica o Prorocolo ModBus a partir dos dados recebidos
/// @param modbusLine Estrutura de dados ModBus a ser preenchida
/// @param data dados recebidos via UART
void decodeModBus(ModBusData_t *modbusLine, uint8_t *data, uint16_t len);

/// @brief CRC de 16 bits
/// @param data dados para cálculo do CRC
/// @param len tamanho dos dados
/// @return o valor CRC calculado
uint16_t crc16_ccitt(const uint8_t *data, size_t len);


void VTaskModBus(void *pvArgs);

#endif