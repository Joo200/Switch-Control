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

#include "GpioConfig.h"

#include <esp_log.h>

#include <fstream>

namespace config {

gpio_num_t ConfigGpio::gpio() const {
    if (kGpioMap.find(channel) == kGpioMap.end()) {
        ESP_LOGE("Gpio", "Unknown gpio requested for channel %s", channel.c_str());
        return GPIO_NUM_0;
    }
    return kGpioMap.find(channel)->second.gpio;
}

bool ConfigGpio::hasCapability() const {
    int cap = 0;
    switch (type) {
        case ChannelType::eI2c:
            cap = CAP_I2C;
            break;
        case ChannelType::eServo:
            cap = CAP_SERVO_OUT;
            break;
        case ChannelType::eSmartButton:
            cap = CAP_SMART_BUTTON;
            break;
        default:
            break;
    }
    if (cap == 0) return true;

    return (kGpioMap.find(channel)->second.capabilities & cap) != 0;
}

void ConfigGpio::validate() const {
    auto gpioEntry = kGpioMap.find(channel);
    if (gpioEntry == kGpioMap.end()) {
        throw std::runtime_error("unknown gpio entry specified");
    }

    if (!hasCapability()) {
        throw std::runtime_error("no capability");
    }

    switch (type) {
        case ChannelType::eDisabled:
        case ChannelType::eInvalid:
        default:
            break;
        case ChannelType::eServo:
            if (!servoCfg_.has_value()) {
                throw std::runtime_error("No servo configuration provided");
            }
            servoCfg_->validate();
            break;
        case ChannelType::eSmartButton:
            if (!buttonCfg_.has_value()) {
                throw std::runtime_error("No button configuration provided");
            }
            buttonCfg_->validate();
            break;
    }
}

void to_json(nlohmann::json &j, const ConfigGpio &ch) {
    j["channel"] = ch.channel;
    j["type"] = ch.type;

    if (ch.buttonCfg_) {
        j["button"] = *ch.buttonCfg_;
    }

    if (ch.servoCfg_) {
        j["servo"] = *ch.servoCfg_;
    }
}

void from_json(const nlohmann::json &j, ConfigGpio &ch) {
    j.at("channel").get_to(ch.channel);
    j.at("type").get_to(ch.type);

    if (j.count("button")) {
        ch.buttonCfg_ = j.at("button").get<ConfigButton>();
    }

    if (j.count("servo")) {
        ch.servoCfg_ = j.at("servo").get<ConfigServo>();
    }
}

static const inline std::string kBasePath = "/spiffs/";

config::ConfigGpio readGpio(const std::string &gpio) {
    std::string path = kBasePath + gpio + ".json";
    std::ifstream f(path);
    if (!f.is_open()) {
        ESP_LOGW("Config", "Unable to read gpio configuration for %s, storing default", gpio.c_str());
        config::ConfigGpio cfg{};
        cfg.channel = gpio;
        writeGpio(cfg);
        return cfg;
    }
    ESP_LOGI("Config", "Reading stored configuration for gpio %s from disk", gpio.c_str());
    nlohmann::json json = nlohmann::json::parse(f);
    f.close();
    ESP_LOGI("Config", "Loaded '%s'", json.dump().c_str());

    try {
        config::ConfigGpio data = json;
        data.validate();
        return data;
    } catch (std::exception &e) {
        ESP_LOGE("Config", "Stored configuration for channel %s is not valid: %s", gpio.c_str(), e.what());
        config::ConfigGpio cfg{};
        cfg.channel = gpio;
        writeGpio(cfg);
        return cfg;
    }
}

void writeGpio(const config::ConfigGpio &cfg) {
    ESP_LOGI("Config", "Storing new gpio configuration for %s", cfg.channel.c_str());
    std::string path = kBasePath + cfg.channel + ".json";
    std::ofstream f(path);
    if (!f.is_open()) {
        ESP_LOGE("Config", "Opening configuration file %s failed", path.c_str());
    }
    nlohmann::json j = cfg;
    f << j << std::endl;
    f.close();
}

}  // namespace config