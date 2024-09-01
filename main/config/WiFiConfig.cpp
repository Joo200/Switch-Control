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

#include "WiFiConfig.h"

#include <esp_log.h>

#include <fstream>

namespace config {
static const inline std::string kWiFiPath = "/spiffs/wifi.json";

config::WiFiConfig readWiFi() {
    std::ifstream f(kWiFiPath);
    if (!f.is_open()) {
        ESP_LOGW("Config", "Unable to read wifi configuration, storing default");
        config::WiFiConfig cfg{};
        writeWiFi(cfg);
        return cfg;
    }
    ESP_LOGI("Config", "Reading stored configuration from disk");
    try {
        config::WiFiConfig data = nlohmann::json::parse(f);
        f.close();
        return data;
    } catch (const std::exception &e) {
        ESP_LOGW("Config", "Invalid wifi configuration stored, resetting config.");
        f.close();
        config::WiFiConfig cfg{};
        writeWiFi(cfg);
        return cfg;
    }
}

void writeWiFi(const config::WiFiConfig &cfg) {
    ESP_LOGI("Config", "Storing new wifi configuration");
    std::ofstream f(kWiFiPath);
    if (!f.is_open()) {
        ESP_LOGE("Config", "Opening configuration file %s failed", kWiFiPath.c_str());
    }
    nlohmann::json j = cfg;
    f << j << std::endl;
    f.close();
}
bool WiFiConfig::operator==(const WiFiConfig &rhs) const {
    return mode == rhs.mode && hostname == rhs.hostname && sta == rhs.sta && ap == rhs.ap;
}
bool WiFiConfig::operator!=(const WiFiConfig &rhs) const { return !(rhs == *this); }

bool WiFiClientConfig::operator==(const WiFiClientConfig &rhs) const {
    return ssid == rhs.ssid && passphrase == rhs.passphrase && method == rhs.method && staticIp == rhs.staticIp;
}
bool WiFiClientConfig::operator!=(const WiFiClientConfig &rhs) const { return !(rhs == *this); }

bool WiFiApConfig::operator==(const WiFiApConfig &rhs) const {
    return ssid == rhs.ssid && passphrase == rhs.passphrase;
}
bool WiFiApConfig::operator!=(const WiFiApConfig &rhs) const { return !(rhs == *this); }

bool StaticConfiguration::operator==(const StaticConfiguration &rhs) const {
    return address == rhs.address && gateway == rhs.gateway && netmask == rhs.netmask;
}
bool StaticConfiguration::operator!=(const StaticConfiguration &rhs) const { return !(rhs == *this); }
}  // namespace config