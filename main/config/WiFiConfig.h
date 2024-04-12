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

#ifndef SWITCHCONTROL_CONFIG_WIFICONFIG_H
#define SWITCHCONTROL_CONFIG_WIFICONFIG_H

#include <nlohmann/json.hpp>
#include <string>

namespace config {

enum class WiFiMode {
    eOff,
    eSta,
    eAp
};

NLOHMANN_JSON_SERIALIZE_ENUM(WiFiMode, {
                                                  {WiFiMode::eOff, "Off"},
                                                  {WiFiMode::eSta, "STA"},
                                                  {WiFiMode::eAp, "AP"},
                                              });

enum class IpMethod {
    eDhcp,
    eStatic
};

NLOHMANN_JSON_SERIALIZE_ENUM(IpMethod, {
{IpMethod::eDhcp, "dhcp"},
{IpMethod::eStatic, "static"},
});

struct StaticConfiguration {
    std::string address{"192.168.178.30"};
    std::string gateway{"192.168.178.1"};
    std::string netmask{"255.255.255.0"};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StaticConfiguration, address, gateway, netmask);

struct WiFiClientConfig {
    std::string ssid;
    std::string passphrase;
    IpMethod method{IpMethod::eDhcp};
    StaticConfiguration staticIp{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WiFiClientConfig, ssid, passphrase, method, staticIp);

struct WiFiApConfig {
    std::string ssid{"switch-control"};
    std::string passphrase{"emergency"};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WiFiApConfig, ssid, passphrase);

struct WiFiConfig {
    WiFiMode mode{WiFiMode::eAp};
    std::string hostname{"switch-control"};
    WiFiClientConfig sta{};
    WiFiApConfig ap{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WiFiConfig, mode, hostname, sta, ap);

config::WiFiConfig readWiFi();

void writeWiFi(const config::WiFiConfig &cfg);
}  // namespace config

#endif  // SWITCHCONTROL_CONFIG_WIFICONFIG_H
