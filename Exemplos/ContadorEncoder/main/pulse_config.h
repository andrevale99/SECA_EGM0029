#include <driver/pulse_cnt.h>

#define PULSE_COUNTER_GPIO GPIO_NUM_14
#define PULSE_COUNTER_MIN_LIMITE -10000
#define PULSE_COUNTER_MAX_LIMITE 10000

esp_err_t PULSE_COUNTER_config(pcnt_unit_handle_t *handle_pcnt)
{
    pcnt_unit_config_t unitConfig =
        {
            .high_limit = PULSE_COUNTER_MAX_LIMITE,
            .low_limit = PULSE_COUNTER_MIN_LIMITE,
        };
    ESP_ERROR_CHECK(pcnt_new_unit(&unitConfig, handle_pcnt));

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(*handle_pcnt, &filter_config));

    pcnt_channel_handle_t pcnt_chan_a = NULL;
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = PULSE_COUNTER_GPIO,
        .level_gpio_num = -1,
    };
    ESP_ERROR_CHECK(pcnt_new_channel(*handle_pcnt, &chan_a_config, &pcnt_chan_a));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a,
                                                 PCNT_CHANNEL_EDGE_ACTION_HOLD,
                                                 PCNT_CHANNEL_EDGE_ACTION_INCREASE));

    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a,
                                                  PCNT_CHANNEL_LEVEL_ACTION_HOLD,
                                                  PCNT_CHANNEL_LEVEL_ACTION_KEEP));

    ESP_ERROR_CHECK(pcnt_unit_enable(*handle_pcnt));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(*handle_pcnt));
    ESP_ERROR_CHECK(pcnt_unit_start(*handle_pcnt));

    return ESP_OK;
}