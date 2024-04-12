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

#ifndef SWITCHCONTROL_WIFI_WIFICONTROLLER_H
#define SWITCHCONTROL_WIFI_WIFICONTROLLER_H

#include <esp_netif_types.h>
#include "config/ConfigurationStorage.h"
#include "config/WiFiConfig.h"

namespace wifi {

class WiFiController {
   public:
    explicit WiFiController(const config::WiFiConfig &cfg);

    void tick();

    void updateConfig(const config::WiFiConfig &config);

    nlohmann::json getStatus();

    [[nodiscard]] const config::WiFiConfig &getConfig() { return cfg_; }

   private:
    config::WiFiConfig cfg_;
    esp_netif_t *netif_{nullptr};

    void updateMode();
    void createAP();
    void connectToSta();
    void updateLED() const;

    bool update{false};
};
}  // namespace wifi

#endif  // SWITCHCONTROL_WIFI_WIFICONTROLLER_H
