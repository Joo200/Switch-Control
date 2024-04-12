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

#include "WiFiController.h"

#include <driver/gpio.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <hal/gpio_types.h>
#include <nvs_flash.h>

#include <cstring>
#include <lwip/ip4_addr.h>

int retry_num = 0;
enum class ConnectionState { eUnknown = 0, eStarting = 1, eConnected = 2, eIpReceived = 3, eDisconnected = 4 };

static ConnectionState curr_state = ConnectionState::eUnknown;

NLOHMANN_JSON_SERIALIZE_ENUM(ConnectionState, {
                                                  {ConnectionState::eUnknown, "Unknown"},
                                                  {ConnectionState::eStarting, "Starting"},
                                                  {ConnectionState::eConnected, "Connected"},
                                                  {ConnectionState::eIpReceived, "IpReceived"},
                                                  {ConnectionState::eDisconnected, "Disconnected"},
                                              })

namespace wifi {
static void wifi_event_handler(void *, esp_event_base_t, int32_t event_id, void *) {
    if (event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI("WiFi", "WIFI CONNECTING....");
        curr_state = ConnectionState::eStarting;
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI("WiFi", "WiFi CONNECTED");
        curr_state = ConnectionState::eConnected;
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI("WiFi", "WiFi lost connection");
        curr_state = ConnectionState::eDisconnected;
        if (retry_num < 5) {
            esp_wifi_connect();
            retry_num++;
            ESP_LOGI("WiFi", "Retrying to Connect...");
        }
    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI("WiFi", "Wifi got IP...");
        curr_state = ConnectionState::eIpReceived;
    }
}

void WiFiController::connectToSta() {
    // TODO: Segfault when changing configurations
    esp_wifi_stop();
    retry_num = 0;
    curr_state = ConnectionState::eUnknown;

    ESP_LOGI("WiFi", "Connecting to SSID %s with passphrase %s", cfg_.sta.ssid.c_str(), cfg_.sta.passphrase.c_str());

    netif_ = esp_netif_create_default_wifi_sta();
    if (!cfg_.hostname.empty()) {
        esp_netif_set_hostname(netif_, cfg_.hostname.c_str());
    }
    if (cfg_.sta.method == config::IpMethod::eStatic) {
        esp_netif_dhcpc_stop(netif_);
        esp_netif_ip_info_t ipv4;
        memset(&ipv4, 0, sizeof(esp_netif_ip_info_t));
        esp_netif_str_to_ip4(cfg_.sta.staticIp.address.c_str(), &ipv4.ip);
        esp_netif_str_to_ip4(cfg_.sta.staticIp.gateway.c_str(), &ipv4.gw);
        esp_netif_str_to_ip4(cfg_.sta.staticIp.netmask.c_str(), &ipv4.netmask);
        esp_netif_set_ip_info(netif_, &ipv4);
    }

    wifi_config_t wifi_configuration;
    memset(&wifi_configuration, 0, sizeof(wifi_config_t));
    strcpy((char *)wifi_configuration.sta.ssid, cfg_.sta.ssid.c_str());
    strcpy((char *)wifi_configuration.sta.password, cfg_.sta.passphrase.c_str());
    // esp_log_write(ESP_LOG_INFO, "Kconfig", "SSID=%s, PASS=%s", ssid, pass);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}

void WiFiController::createAP() {
    // TODO: Segfault when changing configurations
    esp_wifi_stop();
    retry_num = 0;
    curr_state = ConnectionState::eUnknown;


    netif_ = esp_netif_create_default_wifi_ap();
    if (!cfg_.hostname.empty()) {
        esp_netif_set_hostname(netif_, cfg_.hostname.c_str());
    }

    wifi_config_t wifi_configuration;
    memset(&wifi_configuration, 0, sizeof(wifi_config_t));
    strcpy((char *)wifi_configuration.ap.ssid, cfg_.ap.ssid.c_str());
    strcpy((char *)wifi_configuration.ap.password, cfg_.ap.passphrase.c_str());
    wifi_configuration.ap.channel = 5;
    wifi_configuration.ap.max_connection = 5;
    wifi_configuration.ap.authmode = wifi_auth_mode_t::WIFI_AUTH_WPA2_WPA3_PSK;
    wifi_configuration.ap.pmf_cfg.required = true;

    esp_wifi_set_config(WIFI_IF_AP, &wifi_configuration);

    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_AP);
    ESP_LOGI("WiFi", "wifi_init_softap finished. SSID:%s  password:%s", cfg_.ap.ssid.c_str(), cfg_.ap.passphrase.c_str());
}

WiFiController::WiFiController(const config::WiFiConfig &cfg) : cfg_(cfg) {
    // 2 - Wi-Fi Configuration Phase
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();

    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);  //
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, nullptr);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, nullptr);

    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);

    updateMode();
}

void WiFiController::updateMode() {
    if (netif_ != nullptr) {
        esp_netif_destroy_default_wifi(netif_);
    }
    switch (cfg_.mode) {
        case config::WiFiMode::eOff:
            esp_wifi_stop();
            break;
        case config::WiFiMode::eAp:
            createAP();
            break;
        case config::WiFiMode::eSta:
            connectToSta();
            break;
    }
}

static bool blinkState(int cycle, int value) {
    auto now = std::chrono::steady_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return (millis % cycle) > value;
}

void WiFiController::updateLED() const {
    switch (cfg_.mode) {
        case config::WiFiMode::eOff:
            gpio_set_level(GPIO_NUM_2, 0);
            break;
        case config::WiFiMode::eAp:
            gpio_set_level(GPIO_NUM_2, blinkState(500, 300));
            break;
        case config::WiFiMode::eSta:
            if (curr_state == ConnectionState::eConnected || curr_state == ConnectionState::eIpReceived) {
                gpio_set_level(GPIO_NUM_2, blinkState(2000, 1500));
            } else {
                gpio_set_level(GPIO_NUM_2, blinkState(2000, 500));
            }
            break;
        default:
            gpio_set_level(GPIO_NUM_2, blinkState(250, 50));
    }
}

void WiFiController::tick() {
    if (update) {
        updateMode();
        update = false;
        return;
    }

    static int wifi_ap_lvl = 0;
    if (gpio_get_level(GPIO_NUM_0) == 0) {
        wifi_ap_lvl++;
    } else {
        wifi_ap_lvl = 0;
    }

    if (wifi_ap_lvl == 100) {
        ESP_LOGI("WiFi", "Switching WiFi Mode because button was pressed.");
        switch (cfg_.mode) {
            case config::WiFiMode::eOff:
                cfg_.mode = config::WiFiMode::eSta;
                break;
            case config::WiFiMode::eSta:
                cfg_.mode = config::WiFiMode::eAp;
                break;
            case config::WiFiMode::eAp:
                cfg_.mode = config::WiFiMode::eOff;
                break;
        }
        config::writeWiFi(cfg_);
        updateMode();
    }
    updateLED();
}

void WiFiController::updateConfig(const config::WiFiConfig &config) {
    this->cfg_ = config;
    update = true;
}

nlohmann::json WiFiController::getStatus() {
    nlohmann::json status;
    status["mode"] = cfg_.mode;
    if (cfg_.mode == config::WiFiMode::eSta) {
        status["connected"] = curr_state;
    }
    return status;
}
}  // namespace wifi