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

#include "ConfigurationStorage.h"

#include <esp_log.h>
#include <esp_spiffs.h>

#include <fstream>

namespace config {
static const inline std::string kStoragePath = "/spiffs";

void ConfigurationStorage::setup() {
    if (esp_spiffs_mounted(nullptr)) {
        ESP_LOGW("config", "SPIFFS Already mounted");
    }

    esp_vfs_spiffs_conf_t conf = {
        .base_path = kStoragePath.c_str(), .partition_label = nullptr, .max_files = 20, .format_if_mount_failed = true};

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE("config", "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE("config", "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE("config", "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI("config", "Mount successful");

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(nullptr, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE("config", "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI("config", "Partition size: total: %d, used: %d", total, used);
    }
}

ConfigurationStorage::ConfigurationStorage() {
    for (const auto &item : config::kGpioMap) {
        setConfig(item.first, readGpio(item.first));
    }
}

std::vector<config::ConfigGpio> ConfigurationStorage::getChannels() {
    auto kv = std::views::values(channels_);
    return {kv.begin(), kv.end()};
}
}  // namespace config