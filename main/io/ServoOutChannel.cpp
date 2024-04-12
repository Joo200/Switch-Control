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


#include "ServoOutChannel.h"

#include <driver/ledc.h>
#include <esp_log.h>

#include <cstring>
#include <map>

namespace io {

ServoOutputChannel::ServoOutputChannel(const config::ConfigGpio &config) : config_(config) {
    initChannel();
}

ServoOutputChannel::~ServoOutputChannel() = default;

const std::map<gpio_num_t, ledc_channel_t> kGpioToLedCChannelMap = {
    {GPIO_NUM_25, LEDC_CHANNEL_0}, {GPIO_NUM_13, LEDC_CHANNEL_1}, {GPIO_NUM_23, LEDC_CHANNEL_2},
    {GPIO_NUM_19, LEDC_CHANNEL_3}, {GPIO_NUM_18, LEDC_CHANNEL_4}, {GPIO_NUM_17, LEDC_CHANNEL_5},
    {GPIO_NUM_16, LEDC_CHANNEL_6}, {GPIO_NUM_4, LEDC_CHANNEL_7},
};

void ServoOutputChannel::initLedc() {
    ESP_LOGI("Servo", "Initializing LEDC");

    ledc_timer_config_t timer_conf{};
    memset(&timer_conf, 0, sizeof(ledc_timer_config_t));
    timer_conf.clk_cfg = LEDC_AUTO_CLK;
    timer_conf.duty_resolution = LEDC_TIMER_15_BIT;
    timer_conf.freq_hz = 50;
    timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer_conf.timer_num = LEDC_TIMER_0;
    ledc_timer_config(&timer_conf);

    ESP_LOGI("Servo", "Initializing LEDC finished");
}

static int getDuty(int us) {
    const int kBitSize = 15;
    return (1 << kBitSize) * us / 20000;
}

void ServoOutputChannel::initChannel() const {
    ESP_LOGI("Servo", "Initializing Channel %s", config_.channel.c_str());
    gpio_reset_pin(config_.gpio());

    ledc_channel_config_t channel_conf{};
    memset(&channel_conf, 0, sizeof(ledc_timer_config_t));
    channel_conf.channel = kGpioToLedCChannelMap.at(config_.gpio());
    channel_conf.duty = getDuty(config_.servoCfg_->servoLeft);
    channel_conf.gpio_num = config_.gpio();
    channel_conf.intr_type = LEDC_INTR_DISABLE;
    channel_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    channel_conf.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&channel_conf);
    ESP_LOGI("Servo", "Initializing Channel %s finished", config_.channel.c_str());
}

void ServoOutputChannel::setServo(int us) {
    ESP_LOGI("Servo", "Set Servo %s to state %d us; dc %d", config_.channel.c_str(), us, getDuty(us));
    ledc_channel_t ledcChannel = kGpioToLedCChannelMap.at(config_.gpio());
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, ledcChannel, getDuty(us));
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, ledcChannel);
    currPos_ = us;
}

void ServoOutputChannel::actionLeft() {
    setServo(config_.servoCfg_->servoOverdrawLeft);
    overdrawTime_ = std::chrono::steady_clock::now();
    currDir_ = config::SwitchDirection::eLeft;
    overdraw_ = true;
}

void ServoOutputChannel::actionRight() {
    setServo(config_.servoCfg_->servoOverdrawRight);
    overdrawTime_ = std::chrono::steady_clock::now();
    currDir_ = config::SwitchDirection::eRight;
    overdraw_ = true;
}

void ServoOutputChannel::checkOverdraw() {
    if (!overdraw_) return;
    int newPos = -1;
    if (currDir_ == config::SwitchDirection::eRight) {
        newPos = config_.servoCfg_->servoRight;
    } else if (currDir_ == config::SwitchDirection::eLeft) {
        newPos = config_.servoCfg_->servoLeft;
    }
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> lastChange = now - overdrawTime_;
    if (lastChange.count() > config_.servoCfg_->overdrawTime) {
        setServo(newPos);
        overdraw_ = false;
    }
}
void ServoOutputChannel::executePendingAction() {
    if (!pendingAction_.has_value()) {
        return;
    }
    switch (pendingAction_->direction) {
        case config::SwitchDirection::eUnknown:
        case config::SwitchDirection::eInvalid:
        default:
            break;
        case config::SwitchDirection::eLeft:
            actionLeft();
            break;
        case config::SwitchDirection::eRight:
            actionRight();
            break;
        case config::SwitchDirection::eCustom:
            setServo(pendingAction_->customTime);
            currDir_ = config::SwitchDirection::eCustom;
            overdraw_ = false;
            break;
    }
    removePendingAction();
}

void to_json(nlohmann::json &j, const ServoOutputChannel &ch) {
    j["time"] = ch.getCurrPos();
    j["position"] = ch.getDirection();
    auto pending = ch.getPendingAction();
    if (pending.has_value()) {
        j["nextPosition"] = pending->direction;
    }
    j["overdrawing"] = ch.isOverdrawing();
}

}  // namespace io