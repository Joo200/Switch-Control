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


#ifndef SWITCHCONTROL_IO_SMARTBUTTONCHANNEL_H
#define SWITCHCONTROL_IO_SMARTBUTTONCHANNEL_H

#include <deque>
#include <map>

#include "ServoOutChannel.h"
#include "config/ButtonConfig.h"
#include "config/GpioConfig.h"
#include "config/ServoConfig.h"

namespace io {
enum class MatchingState { eNoMatch = 0, eMatch = 1, ePending = 2 };

class SmartButtonChannel {
   public:
    const inline static int kRequiredTicks = 3;

    explicit SmartButtonChannel(const config::ConfigGpio &config);
    ~SmartButtonChannel();

    [[nodiscard]] bool tickButton();

    void updateMatchingState(const std::map<std::string, io::ServoOutputChannel> &channels);

    [[nodiscard]] std::vector<config::SwitchAction> getAction() { return config_.buttonCfg_->actionOnPress; }

   private:
    const config::ConfigGpio config_;

    MatchingState matches_{MatchingState::ePending};
    int tickPressed_{0};

    void setButton(bool val);
};
}  // namespace io

#endif  // SWITCHCONTROL_IO_SMARTBUTTONCHANNEL_H
