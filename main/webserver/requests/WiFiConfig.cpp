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

#include "config/WiFiConfig.h"

namespace httpserver::requests {

inline static const char *kWiFiPath = "/api/wifi";

WiFiGet::WiFiGet(ConfigurationServer &srv)
    : AbstractRequestHandler(srv, kWiFiPath, HTTP_GET) {}

esp_err_t WiFiGet::handleRequest(httpd_req_t *req) {
    ESP_LOGI("http", "getting wifi configuration");
    sendJsonAnswer(req, srv_.getWifi().getConfig());
    return ESP_OK;
}

WiFiSet::WiFiSet(ConfigurationServer &srv) : AbstractRequestHandler(srv, kWiFiPath, HTTP_POST) {}

esp_err_t WiFiSet::handleRequest(httpd_req_t *req) {
    try {
        config::WiFiConfig cfg = getJsonBody(req);
        config::writeWiFi(cfg);
        srv_.getWifi().updateConfig(cfg);
        sendJsonAnswer(req, {{"Status", "Accepted"}});
    } catch (const std::exception &e) {
        ESP_LOGW("http", "Unable to process wifi configuration: %s", e.what());
        httpd_resp_send_500(req);
    }
    return ESP_OK;
}
}  // namespace httpserver::requests
