// crc_examples.c
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

// ---------- CRC-8 (poly 0x07, init 0x00) ----------
uint8_t crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;            // init
    const uint8_t poly = 0x07;    // x^8 + x^2 + x + 1
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b) {
            if (crc & 0x80)
                crc = (uint8_t)((crc << 1) ^ poly);
            else
                crc <<= 1;
        }
    }
    return crc;
}

// ---------- CRC-16-CCITT (poly 0x1021, init 0xFFFF) ----------
uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;       // common init (variant: 0x0000 or 0x1D0F exist)
    const uint16_t poly = 0x8005;
    for (size_t i = 0; i < len; ++i) {
        crc ^= (uint16_t)data[i] << 8;
        for (int b = 0; b < 8; ++b) {
            if (crc & 0x8000)
                crc = (uint16_t)((crc << 1) ^ poly);
            else
                crc <<= 1;
        }
    }
    return crc;
}

// ---------- CRC-32 (IEEE 802.3) ----------
// We'll generate the table once on first call.
static uint32_t crc32_table[256];
static int crc32_table_initialized = 0;

static void crc32_init_table(void) {
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t c = i;
        for (int j = 0; j < 8; ++j) {
            if (c & 1) c = 0xEDB88320u ^ (c >> 1);
            else      c = c >> 1;
        }
        crc32_table[i] = c;
    }
    crc32_table_initialized = 1;
}

uint32_t crc32(const uint8_t *data, size_t len) {
    if (!crc32_table_initialized) crc32_init_table();
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i) {
        uint8_t idx = (uint8_t)((crc ^ data[i]) & 0xFFu);
        crc = (crc >> 8) ^ crc32_table[idx];
    }
    return crc ^ 0xFFFFFFFFu;
}

// ---------- Example usage ----------
int main(void) {
    const uint8_t message[] = "Exemplo de CRC";
    size_t len = sizeof(message) - 1; // sem contar '\0'

    uint8_t  c8  = crc8(message, len);
    uint16_t c16 = crc16_ccitt(message, len);
    uint32_t c32 = crc32(message, len);

    printf("Mensagem: \"%s\"\n", message);
    printf("CRC-8   : 0x%02X\n", c8);
    printf("CRC-16  : 0x%04X\n", c16);
    printf("CRC-32  : 0x%08X\n", c32);

    return 0;
}
