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


#ifndef SWITCHCONTROL_CONTROLLER_OPERATIONCONTROLLER_H
#define SWITCHCONTROL_CONTROLLER_OPERATIONCONTROLLER_H

#include <queue>
#include <vector>

#include "config/ConfigurationStorage.h"
#include "config/ServoConfig.h"
#include "io/ServoOutChannel.h"
#include "io/SmartButtonChannel.h"

class OperationController {
   public:
    const inline static double kWaitDurationBetweenNextDirChange = 2;

    OperationController();

    ~OperationController() = default;

    void addNewChannel(const config::ConfigGpio &cfg);
    void updateChannel(const config::ConfigGpio &cfg);

    void requestSwitchChange(const std::vector<config::SwitchAction> &req);
    void forceSwitchChange(config::SwitchAction &req);

    nlohmann::json generateStatus();

    void tick();

   private:
    // config::ConfigurationStorage cfg_;

    std::mutex changeMutex_;

    std::chrono::steady_clock::time_point lastDirChange_{std::chrono::steady_clock::now()};

    std::map<std::string, io::SmartButtonChannel> buttonChannels_;
    std::map<std::string, io::ServoOutputChannel> servoOutChannels_;

    void performNextServoChange();
    void performButtonTicks();
    void performAction(io::ServoOutputChannel &pendingChange);
};

#endif  // SWITCHCONTROL_CONTROLLER_OPERATIONCONTROLLER_H
