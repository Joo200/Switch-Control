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

#ifndef SWITCHCONTROL_CONFIG_SERVOCONFIG_H
#define SWITCHCONTROL_CONFIG_SERVOCONFIG_H

#include <nlohmann/json.hpp>
#include <string>

namespace config {
const static inline int kMinServoTime = 800;
const static inline int kMaxServoTime = 2200;

class ConfigServo {
   public:
    int servoLeft{1300};            ///< Time in us for left position
    int servoRight{1700};          ///< Time in us for right position
    int servoOverdrawLeft{1250};    ///< Time in us for left overdraw position
    int servoOverdrawRight{1750};  ///< Time in us for right overdraw position
    double overdrawTime{0.2};      ///< Time in seconds to overdraw

    void validate() const;
};

enum class SwitchDirection { eInvalid = -1, eLeft = 1, eRight = 2, eUnknown = 3, eCustom = 4 };

NLOHMANN_JSON_SERIALIZE_ENUM(SwitchDirection, {
                                                  {SwitchDirection::eInvalid, nullptr},
                                                  {SwitchDirection::eUnknown, "Unknown"},
                                                  {SwitchDirection::eLeft, "Left"},
                                                  {SwitchDirection::eRight, "Right"},
                                                  {SwitchDirection::eCustom, "Custom"}
                                              })

class SwitchAction {
   public:
    std::string ip{};
    std::string channel{"0"};
    SwitchDirection direction{SwitchDirection::eUnknown};
    int customTime{1500};

    void validate() const;
};

bool isValidServoTime(int time);

void to_json(nlohmann::json &j, const ConfigServo &ch);
void from_json(const nlohmann::json &j, ConfigServo &ch);

void to_json(nlohmann::json &j, const SwitchAction &a);
void from_json(const nlohmann::json &j, SwitchAction &a);
}  // namespace config

#endif  // SWITCHCONTROL_CONFIG_SERVOCONFIG_H
