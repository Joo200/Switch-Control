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

#ifndef SWITCHCONTROL_CONFIG_CONFIGURATIONSTORAGE_H
#define SWITCHCONTROL_CONFIG_CONFIGURATIONSTORAGE_H

#include <exception>
#include <map>
#include <string>

#include "GpioConfig.h"
#include "WiFiConfig.h"

namespace config {

class ConfigurationStorage {
   public:
    explicit ConfigurationStorage();

    static void setup();

    [[nodiscard]] bool hasConfig(const std::string &channel) { return channels_.find(channel) != channels_.end(); }

    void setConfig(const std::string &key, const config::ConfigGpio &conf) {
        channels_[key] = conf;
        config::writeGpio(conf);
    }

    [[nodiscard]] const config::ConfigGpio &getConfig(const std::string &channel) { return channels_[channel]; }
    [[nodiscard]] std::vector<config::ConfigGpio> getChannels();

   private:
    std::map<std::string, config::ConfigGpio> channels_;
};
}  // namespace config

#endif  // SWITCHCONTROL_CONFIG_CONFIGURATIONSTORAGE_H
