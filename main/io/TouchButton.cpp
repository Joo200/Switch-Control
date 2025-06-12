/*
* Copyright © 2024 Johannes Zangl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "TouchButton.h"

#include <freertos/FreeRTOS.h>
#include "esp_log.h"
#include "driver/touch_pad.h"

#define TOUCH_THRESH_PERCENT  80      // 80% of the untouched baseline value

std::map<gpio_num_t, touch_pad_t> kTouchMap = {
    {GPIO_NUM_1, TOUCH_PAD_NUM1},
{GPIO_NUM_2, TOUCH_PAD_NUM2},
{GPIO_NUM_3, TOUCH_PAD_NUM3},
{GPIO_NUM_4, TOUCH_PAD_NUM4},
{GPIO_NUM_5, TOUCH_PAD_NUM5},
{GPIO_NUM_6, TOUCH_PAD_NUM6},
{GPIO_NUM_7, TOUCH_PAD_NUM7},
{GPIO_NUM_8, TOUCH_PAD_NUM8},
{GPIO_NUM_9, TOUCH_PAD_NUM9},
{GPIO_NUM_10, TOUCH_PAD_NUM10},
{GPIO_NUM_11, TOUCH_PAD_NUM11},
{GPIO_NUM_12, TOUCH_PAD_NUM12}
};

namespace io {
    TouchButton::TouchButton(const config::ConfigGpio &config) : touch_(kTouchMap[config.gpio()]) {
        ESP_ERROR_CHECK(touch_pad_init());
        touch_pad_denoise_t denoise = {
            /* The bits to be cancelled are determined according to the noise level. */
            .grade = TOUCH_PAD_DENOISE_BIT4,
            .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
        };
        touch_pad_denoise_set_config(&denoise);
        touch_pad_denoise_enable();

        ESP_ERROR_CHECK(touch_pad_config(touch_));
#if 0
        touch_pad_set_voltage(TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD);
        touch_pad_set_idle_channel_connect(TOUCH_PAD_IDLE_CH_CONNECT_DEFAULT);
        touch_pad_set_cnt_mode(touch_, TOUCH_PAD_SLOPE_DEFAULT, TOUCH_PAD_TIE_OPT_DEFAULT);
#endif

        // Set fsm mode to timer
        touch_pad_set_charge_discharge_times(0x1000);
        touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
        touch_pad_fsm_start();

        // Wait for a few samples to get a stable baseline
        vTaskDelay(pdMS_TO_TICKS(200));

        // Read the baseline value (untouched)
        uint32_t touch_value;
        ESP_ERROR_CHECK(touch_pad_read_raw_data(touch_, &touch_value));
        touch_threshold_ = touch_value * 15 / 10;

        ESP_LOGI("TOUCH", "Baseline value: %d, threshold: %d", (int)touch_value, (int)touch_threshold_);
    }

    bool TouchButton::tickButton() {
        uint32_t value = 0;
        ESP_ERROR_CHECK(touch_pad_read_raw_data(touch_, &value));
        ESP_LOGI("TOUCH", "Touch value is %d", (int)value);
        if (value > touch_threshold_) {
            tickPressed_++;
        } else {
            tickPressed_ = 0;
        }
        return tickPressed_ == kRequiredTicks;
    }

}
