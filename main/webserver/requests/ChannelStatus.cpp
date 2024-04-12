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

#include "ChannelStatus.h"

#include <esp_log.h>

namespace httpserver::requests {
inline static const char *kConfigPath = "/api/channel";

ChannelStatusGet::ChannelStatusGet(ConfigurationServer &srv)
    : AbstractRequestHandler(srv, kConfigPath, HTTP_GET) {}

esp_err_t ChannelStatusGet::handleRequest(httpd_req_t *req) {
    sendJsonAnswer(req, srv_.getController().generateStatus());
    return ESP_OK;
}

ChannelStatusPost::ChannelStatusPost(ConfigurationServer &srv)
    : AbstractRequestHandler(srv, kConfigPath, HTTP_POST) {}

esp_err_t ChannelStatusPost::handleRequest(httpd_req_t *req) {
    try {
        config::SwitchAction payload = getJsonBody(req);
        payload.validate();
        if (payload.direction == config::SwitchDirection::eCustom) {
           srv_.getController().forceSwitchChange(payload);
           ESP_LOGI("http", "Received new custom channel status, enforcing new state.");
        } else {
            srv_.getController().requestSwitchChange({payload});
        }
    } catch (const std::exception &e) {
        ESP_LOGW("http", "Failed to set temporary state");
        sendJsonError(req, e.what());
    }
    sendEmptySuccess(req);
    return ESP_OK;
}

}
