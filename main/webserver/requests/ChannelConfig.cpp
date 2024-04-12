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

#include "ChannelConfig.h"

#include <esp_log.h>

#include "nlohmann/json.hpp"

namespace httpserver::requests {
inline static const char *kConfigPath = "/api/config";

ConfigGet::ConfigGet(ConfigurationServer &srv)
    : AbstractRequestHandler(srv, kConfigPath, HTTP_GET) {}

esp_err_t ConfigGet::handleRequest(httpd_req_t *req) {
    const std::string &channel = getParamKey("channel", req);
    if (!channel.empty()) {
        if (!srv_.getStorage().hasConfig(channel)) {
            httpd_resp_send_404(req);
            ESP_LOGW("http", "channel %s not found", channel.c_str());
            return ESP_OK;
        }

        ESP_LOGI("http", "getting configuration for channel %s", channel.c_str());
        sendJsonAnswer(req, srv_.getStorage().getConfig(channel));
        return ESP_OK;
    }

    ESP_LOGI("http", "getting configuration for all channels");
    sendJsonAnswer(req, srv_.getStorage().getChannels());
    return ESP_OK;
}

ConfigSet::ConfigSet(ConfigurationServer &srv)
    : AbstractRequestHandler(srv, kConfigPath, HTTP_POST) {}

esp_err_t ConfigSet::handleRequest(httpd_req_t *req) {
    ESP_LOGI("http", "entering set request with %d byte content len", req->content_len);

    try {
        config::ConfigGpio cfg = getJsonBody(req);
        cfg.validate();
        ESP_LOGI("http", "saving configuration from buf for channel %s", cfg.channel.c_str());
        srv_.getStorage().setConfig(cfg.channel, cfg);
        srv_.getController().updateChannel(cfg);
    } catch (const std::exception &e) {
        ESP_LOGW("http", "Failed to set configuration: %s", e.what());
        sendJsonError(req, e.what());
    }

    sendEmptySuccess(req);
    ESP_LOGI("http", "exiting now with status ok");
    return ESP_OK;
}

}  // namespace httpserver::requests
