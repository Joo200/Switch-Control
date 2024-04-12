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

#include "ServoConfig.h"

#include "GpioConfig.h"

namespace config {
void to_json(nlohmann::json &j, const ConfigServo &ch) {
    j["posLeft"] = ch.servoLeft;
    j["posRight"] = ch.servoRight;
    j["posLeftOverdraw"] = ch.servoOverdrawLeft;
    j["posRightOverdraw"] = ch.servoOverdrawRight;
    j["overdrawTime"] = ch.overdrawTime;
}

void from_json(const nlohmann::json &j, ConfigServo &ch) {
    ch.servoLeft = j.at("posLeft").get<int>();
    ch.servoRight = j.at("posRight").get<int>();
    ch.servoOverdrawLeft = j.at("posLeftOverdraw").get<int>();
    ch.servoOverdrawRight = j.at("posRightOverdraw").get<int>();
    ch.overdrawTime = j.at("overdrawTime").get<double>();
}

void ConfigServo::validate() const {
    if (!isValidServoTime(servoLeft)) {
        throw std::runtime_error("Left servo time is invalid: " + std::to_string(servoLeft));
    }
    if (!isValidServoTime(servoRight)) {
        throw std::runtime_error("Right servo time is invalid: " + std::to_string(servoRight));
    }
    if (!isValidServoTime(servoOverdrawLeft)) {
        throw std::runtime_error("Left overdraw servo time is invalid: " + std::to_string(servoOverdrawLeft));
    }
    if (!isValidServoTime(servoOverdrawRight)) {
        throw std::runtime_error("Right overdraw servo time is invalid: " + std::to_string(servoOverdrawRight));
    }
    if (overdrawTime < 0 || overdrawTime > 5) {
        throw std::runtime_error("Overdraw time invalid: " + std::to_string(overdrawTime));
    }
}

bool isValidServoTime(int time) {
    return time >= kMinServoTime && time <= kMaxServoTime;
}

void to_json(nlohmann::json &j, const SwitchAction &a) {
    j["channel"] = a.channel;
    j["direction"] = a.direction;
    if (!a.ip.empty()) j["ip"] = a.ip;
    if (a.customTime != 0) j["time"] = a.customTime;
}

void from_json(const nlohmann::json &j, SwitchAction &a) {
    a.channel = j.at("channel").get<std::string>();
    a.direction = j.at("direction").get<SwitchDirection>();
    if (j.count("ip")) a.ip = j.at("ip").get<std::string>();
    if (j.count("time")) a.customTime = j.at("time").get<int>();
}

void SwitchAction::validate() const {
    if (config::kGpioMap.find(channel) == config::kGpioMap.end()) {
        throw std::runtime_error("Provided channel is invalid.");
    }

    if (direction == SwitchDirection::eUnknown || direction == SwitchDirection::eInvalid) {
        throw std::runtime_error("Provided direction is invalid");
    }

    if (direction == SwitchDirection::eCustom && !isValidServoTime(customTime)) {
        throw std::runtime_error("Provided time is out of range.");
    }
}
}  // namespace config