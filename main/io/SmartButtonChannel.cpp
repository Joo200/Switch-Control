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


#include "SmartButtonChannel.h"

#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>

namespace io {
SmartButtonChannel::SmartButtonChannel(const config::ConfigGpio &config) : config_(config) {
    gpio_reset_pin(config_.gpio());
}

SmartButtonChannel::~SmartButtonChannel() = default;

void SmartButtonChannel::setButton(bool val) {
    gpio_set_level(config_.gpio(), (config_.buttonCfg_->invertedOutput && val) ? 0 : 1);
}

static bool currentBlinkState() {
    auto now = std::chrono::steady_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return (millis % 1000) > 500;
}

bool SmartButtonChannel::tickButton() {
    // Check the state of the button:
    gpio_set_level(config_.gpio(), 0);
    gpio_set_direction(config_.gpio(), GPIO_MODE_INPUT);

    vTaskDelay(1 / portTICK_PERIOD_MS);
    if (gpio_get_level(config_.gpio()) ^ config_.buttonCfg_->invertedInput) {
        tickPressed_++;
    } else {
        tickPressed_ = 0;
    }

    gpio_set_direction(config_.gpio(), GPIO_MODE_OUTPUT_OD);
    if (matches_ == MatchingState::ePending) {
        setButton(currentBlinkState());
    } else if (matches_ == MatchingState::eMatch) {
        setButton(true);
    } else {
        setButton(false);
    }

    return tickPressed_ == kRequiredTicks;
}

void SmartButtonChannel::updateMatchingState(
    const std::map<std::string, io::ServoOutputChannel> &channels) {
    if (config_.buttonCfg_->actionOnPress.empty()) {
        matches_ = MatchingState::eNoMatch;
        return;
    }

    int pendingCount = 0;
    int noMatchesCount = 0;
    for (const auto &item : config_.buttonCfg_->actionOnPress) {
        auto ch = channels.find(item.channel);
        if (ch == channels.end()) {
            continue;
        }
        auto pending = ch->second.getPendingAction();
        config::SwitchDirection current = ch->second.getDirection();
        if (pending.has_value() && pending->direction == item.direction) {
            pendingCount++;
        } else if (current != config::SwitchDirection::eUnknown && current != item.direction) {
            noMatchesCount++;
        }
    }
    if (noMatchesCount > 0) {
        matches_ = MatchingState::eNoMatch;
    } else if (pendingCount > 0) {
        matches_ = MatchingState::ePending;
    } else {
        matches_ = MatchingState::eMatch;
    }
}

}  // namespace io
