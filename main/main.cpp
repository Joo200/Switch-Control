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

#include <esp_log.h>
#include <hal/efuse_hal.h>

#include "controller/OperationController.h"
#include "freertos/FreeRTOS.h"
#include "webserver/ConfigurationServer.h"
#include "wifi/WiFiController.h"

[[noreturn]] void start_main(void) {
    ESP_LOGI("Start", "Starting on Chip with rev %" PRIu32 ".%" PRIu32, efuse_hal_get_major_chip_version(),
             efuse_hal_get_minor_chip_version());
    config::ConfigurationStorage::setup();
    OperationController ctrl;

    config::ConfigurationStorage storage;

    config::WiFiConfig cfg = config::readWiFi();
    wifi::WiFiController wifi(cfg);
    httpserver::ConfigurationServer server(storage, wifi, ctrl);
    if (!server.start()) {
        esp_restart();
    }

    for (const auto &item : storage.getChannels()) {
        ESP_LOGI("Start", "Initializing channel %s", item.channel.c_str());
        ctrl.addNewChannel(item);
    }

    while (true) {
        ctrl.tick();
        wifi.tick();
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

extern "C" {
void app_main(void) { start_main(); }
}