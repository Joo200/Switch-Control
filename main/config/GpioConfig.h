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


#ifndef SWITCHCONTROL_CONFIG_GPIOCONFIG_H
#define SWITCHCONTROL_CONFIG_GPIOCONFIG_H

#include <soc/gpio_num.h>

#include <memory>
#include <nlohmann/json.hpp>

#include "ButtonConfig.h"
#include "ServoConfig.h"

#define CAP_SMART_BUTTON (0x1 << 1)
#define CAP_SERVO_OUT (0x1 << 2)
#define CAP_I2C (0x1 << 3)
#define CAP_SER_REM_IN (0x1 << 4)

namespace config {
enum class ChannelType { eInvalid = -1, eDisabled = 0, eServo = 1, eSmartButton = 2, eI2c = 3 };

NLOHMANN_JSON_SERIALIZE_ENUM(ChannelType, {
                                              {ChannelType::eInvalid, nullptr},
                                              {ChannelType::eDisabled, "Disabled"},
                                              {ChannelType::eServo, "Servo"},
                                              {ChannelType::eSmartButton, "SmartButton"},
                                              {ChannelType::eI2c, "I2c"},
                                          })

struct PinCapabilities {
    gpio_num_t gpio;
    int capabilities;
};

const static inline std::map<std::string, PinCapabilities> kGpioMap = {
    {"A1", {GPIO_NUM_25, CAP_SMART_BUTTON | CAP_SERVO_OUT}},
    {"A2", {GPIO_NUM_13, CAP_SMART_BUTTON | CAP_SERVO_OUT}},
    {"A3", {GPIO_NUM_23, CAP_SMART_BUTTON | CAP_SERVO_OUT}},
    {"A4", {GPIO_NUM_19, CAP_SMART_BUTTON | CAP_SERVO_OUT}},
    {"A5", {GPIO_NUM_18, CAP_SMART_BUTTON | CAP_SERVO_OUT}},
    {"A6", {GPIO_NUM_17, CAP_SMART_BUTTON | CAP_SERVO_OUT}},
    {"A7", {GPIO_NUM_16, CAP_SMART_BUTTON | CAP_SERVO_OUT}},
    {"A8", {GPIO_NUM_4, CAP_SMART_BUTTON | CAP_SERVO_OUT}},
    {"B1", {GPIO_NUM_22, CAP_SMART_BUTTON | CAP_I2C}},
    {"B2", {GPIO_NUM_21, CAP_SMART_BUTTON | CAP_I2C}},
    {"B3", {GPIO_NUM_32, CAP_SMART_BUTTON}},
    {"B4", {GPIO_NUM_33, CAP_SMART_BUTTON}},
    {"B5", {GPIO_NUM_26, CAP_SMART_BUTTON}},
    {"B6", {GPIO_NUM_27, CAP_SMART_BUTTON}},
    {"B7", {GPIO_NUM_14, CAP_SMART_BUTTON}},
    {"B8", {GPIO_NUM_15, CAP_SMART_BUTTON}},
};

class ConfigGpio {
   public:
    std::string channel{"0"};
    ChannelType type{ChannelType::eDisabled};

    std::optional<config::ConfigButton> buttonCfg_;
    std::optional<config::ConfigServo> servoCfg_;

    [[nodiscard]] gpio_num_t gpio() const;

    [[nodiscard]] bool hasCapability() const;

    void validate() const;
};

void to_json(nlohmann::json &j, const ConfigGpio &ch);

void from_json(const nlohmann::json &j, ConfigGpio &ch);

config::ConfigGpio readGpio(const std::string &gpio);

void writeGpio(const config::ConfigGpio &cfg);
}  // namespace config
#endif  // SWITCHCONTROL_CONFIG_GPIOCONFIG_H
