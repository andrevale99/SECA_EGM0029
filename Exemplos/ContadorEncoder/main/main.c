#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <driver/pulse_cnt.h>
#include <driver/gpio.h>
#include <driver/ledc.h>

#include "pwm_config.h"
#include "pulse_config.h"

pcnt_unit_handle_t handlePulseCounter = NULL;

void app_main(void)
{
    PULSE_COUNTER_config(&handlePulseCounter);
    PWM_config();

    int pulsos = 0;
    uint32_t i32ValorPWM = 0;
    while (1)
    {
        pcnt_unit_get_count(handlePulseCounter, &pulsos);
        ESP_LOGI("[MAIN]", "Pulses: %i", pulsos);

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, i32ValorPWM));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0));

        i32ValorPWM += 100;

        if(i32ValorPWM >1023)
            i32ValorPWM = 0;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

