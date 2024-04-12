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


#ifndef SWITCHCONTROL_IO_SERVOOUTCHANNEL_H
#define SWITCHCONTROL_IO_SERVOOUTCHANNEL_H

#include "config/GpioConfig.h"
#include "config/ServoConfig.h"

namespace io {

/**
 * @brief This class represents a single servo output channel
 */
class ServoOutputChannel {
   public:
    explicit ServoOutputChannel(const config::ConfigGpio &config);
    ~ServoOutputChannel();

    static void initLedc();

    void initChannel() const;

    void setPendingAction(const config::SwitchAction &dir) { pendingAction_ = dir; }
    void removePendingAction() { pendingAction_.reset(); }
    [[nodiscard]] std::optional<config::SwitchAction> getPendingAction() const { return pendingAction_; }
    void executePendingAction();

    void checkOverdraw();

    [[nodiscard]] config::SwitchDirection getDirection() const { return currDir_; }
    [[nodiscard]] int getCurrPos() const { return currPos_; }
    [[nodiscard]] bool isOverdrawing() const { return overdraw_; }

   private:
    void actionLeft();
    void actionRight();

    void setServo(int ms);

    const config::ConfigGpio config_;
    std::chrono::steady_clock::time_point overdrawTime_{};

    std::optional<config::SwitchAction> pendingAction_{};
    config::SwitchDirection currDir_{config::SwitchDirection::eUnknown};
    bool overdraw_{false};
    int currPos_{0};
};

void to_json(nlohmann::json &j, const ServoOutputChannel &ch);
}  // namespace io

#endif  // SWITCHCONTROL_IO_SERVOOUTCHANNEL_H
