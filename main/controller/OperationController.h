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

/**
 * @brief This class is the controller to manage changing servo states.
 * It polls every registered buttons and set new values to the available servos.
 */
class OperationController {
   public:
    const inline static double kWaitDurationBetweenNextDirChange = 2;

    /**
     * @brief Create a new controller.
     */
    OperationController();
    ~OperationController() = default;

    /**
     * @brief Add a new channel for a GPIO to the controller.
     * @param cfg the configuration of the gpio
     */
    void addNewChannel(const config::ConfigGpio &cfg);
    /**
     * @brief Update an existing configuration of a GPIO.
     * @param cfg the configuration of the gpio
     */
    void updateChannel(const config::ConfigGpio &cfg);

    /**
     * @brief Request multiple switch change.
     * This will queue the switch change to prevent multiple changes at the same time.
     * @param req list of requested changes
     */
    void requestSwitchChange(const std::vector<config::SwitchAction> &req);
    /**
     * @brief Force a switch change now.
     * Request a change now. This bypasses the queue.
     * @param req the action
     */
    void forceSwitchChange(config::SwitchAction &req);

    /**
     * @brief Create a status of all channels and the controller.
     * @return a json object containing the status
     */
    nlohmann::json generateStatus();

    /**
     * @brief Tick the controller. Should be ticked every 20ms.
     */
    void tick();

   private:
    std::mutex changeMutex_;

    std::chrono::steady_clock::time_point lastDirChange_{std::chrono::steady_clock::now()};

    std::map<std::string, io::SmartButtonChannel> buttonChannels_;
    std::map<std::string, io::ServoOutputChannel> servoOutChannels_;

    void performNextServoChange();
    void performAction(io::ServoOutputChannel &pendingChange);
};

#endif  // SWITCHCONTROL_CONTROLLER_OPERATIONCONTROLLER_H
