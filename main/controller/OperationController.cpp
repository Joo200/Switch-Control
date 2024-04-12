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


#include "OperationController.h"

#include <driver/gpio.h>
#include <esp_log.h>

OperationController::OperationController() { io::ServoOutputChannel::initLedc(); }

void OperationController::addNewChannel(const config::ConfigGpio &cfg) {
    switch (cfg.type) {
        case config::ChannelType::eDisabled:
        default:
            gpio_reset_pin(cfg.gpio());
            break;
        case config::ChannelType::eSmartButton:
            buttonChannels_.insert({cfg.channel, io::SmartButtonChannel(cfg)});
            break;
        case config::ChannelType::eServo: {
            servoOutChannels_.insert({cfg.channel, io::ServoOutputChannel(cfg)});
            break;
        }
    }
}

void OperationController::updateChannel(const config::ConfigGpio &cfg) {
    const std::lock_guard<std::mutex> lock(changeMutex_);
    servoOutChannels_.erase(cfg.channel);
    buttonChannels_.erase(cfg.channel);

    addNewChannel(cfg);
}

void OperationController::forceSwitchChange(config::SwitchAction &req) {
    const std::lock_guard<std::mutex> lock(changeMutex_);

    auto servo = servoOutChannels_.find(req.channel);
    if (servo == servoOutChannels_.end()) {
        ESP_LOGI("Controller", "Skipping change request, unknown servo output channel: %s", req.channel.c_str());
        return;
    }

    servo->second.setPendingAction(req);
    servo->second.executePendingAction();
}

void OperationController::requestSwitchChange(const std::vector<config::SwitchAction> &req) {
    const std::lock_guard<std::mutex> lock(changeMutex_);
    for (const auto &item : req) {
        if (!item.ip.empty()) {
            // TODO: Remote switch change
            continue;
        }

        auto servo = servoOutChannels_.find(item.channel);
        if (servo == servoOutChannels_.end()) {
            ESP_LOGI("Controller", "Skipping change request, unknown servo output channel: %s", item.channel.c_str());
            continue;
        }
        servo->second.removePendingAction();

        if (item.direction != config::SwitchDirection::eCustom && servo->second.getDirection() == item.direction) {
            ESP_LOGI("Controller", "Skipping change request, already in position: %s, %d", item.channel.c_str(),
                     (int)item.direction);
            continue;
        }

        ESP_LOGI("Controller", "Queuing change request: %s, %d", item.channel.c_str(), (int)item.direction);
        servo->second.setPendingAction(item);
    }
}

void OperationController::performAction(io::ServoOutputChannel &pendingChange) {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> lastChange = now - lastDirChange_;
    if (lastChange.count() < kWaitDurationBetweenNextDirChange) {
        ESP_LOGI("Controller", "Pending changes in cooldown, skipping.");
        return;
    }
    const std::lock_guard<std::mutex> lock(changeMutex_);
    lastDirChange_ = now;

    pendingChange.executePendingAction();

    // now populate the changes
    for (auto &item : buttonChannels_) {
        item.second.updateMatchingState(servoOutChannels_);
    }

}

void OperationController::performNextServoChange() {
    for (auto &item : servoOutChannels_) {
        if (item.second.getPendingAction().has_value()) {
            performAction(item.second);
            return;
        }
    }
}

void OperationController::performButtonTicks() {
}

void OperationController::tick() {
    for (auto &item : buttonChannels_) {
        if (item.second.tickButton()) {
            ESP_LOGI("Controller", "Button %s has been pressed, performing change.", item.first.c_str());
            requestSwitchChange(item.second.getAction());
        }
    }

    performNextServoChange();

    for (auto &item : servoOutChannels_) {
        item.second.checkOverdraw();
    }
}

nlohmann::json OperationController::generateStatus() {
    const std::lock_guard<std::mutex> lock(changeMutex_);
    nlohmann::json arr = nlohmann::json::array();
    for (const auto &item : servoOutChannels_) {
        arr.push_back(item.second);
    }
    return arr;
}
