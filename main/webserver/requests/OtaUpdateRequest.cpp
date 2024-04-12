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

#include "OtaUpdateRequest.h"

#include <esp_http_client.h>
#include <esp_https_ota.h>
#include <esp_log.h>

extern const uint8_t cert_start[] asm("_binary_cert_pem_start");
extern const uint8_t cert_end[] asm("_binary_cert_pem_end");

namespace httpserver::requests {
void from_json(const nlohmann::json &j, OtaUpdatePayload &p) { p.url = j.at("url").get<std::string>(); }

OtaUpdateRequest::OtaUpdateRequest(ConfigurationServer &srv) : AbstractRequestHeader(srv, "/api/update", HTTP_POST) {}

static esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD("Update", "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD("Update", "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD("Update", "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD("Update", "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD("Update", "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD("Update", "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD("Update", "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD("Update", "HTTP_EVENT_REDIRECT");
            break;
    }
    return ESP_OK;
}

esp_err_t OtaUpdateRequest::handleRequest(httpd_req_t *req) {
    OtaUpdatePayload payload = getJsonBody(req);

    ESP_LOGI("Update", "Preparing update from %s", payload.url.c_str());

    esp_http_client_config_t config{};
    memset(&config, 0, sizeof(esp_http_client_config_t));
    config.url = payload.url.c_str();
    config.cert_pem = (char *)cert_start;
    config.skip_cert_common_name_check = true;
    config.event_handler = &_http_event_handler;
    config.keep_alive_enable = true;
    esp_https_ota_config_t ota_config{};
    memset(&ota_config, 0, sizeof(esp_https_ota_config_t));
    ota_config.http_config = &config;

    ESP_LOGI("Update", "Updating with ota request.");
    esp_err_t ret = esp_https_ota(&ota_config);

    if (ret == ESP_OK) {
        sendJsonAnswer(req, {{"Status", "Accepted"}});
        ESP_LOGI("Update", "Update was successful, restarting app now.");
        esp_restart();
    } else {
        ESP_LOGW("Update", "Update failed with error %d.", ret);
        httpd_resp_set_status(req, "500");
        sendJsonAnswer(req, {{"Status", "Failed"}, {"Return Key", ret}});
        return ESP_FAIL;
    }
    return ESP_OK;
}

}  // namespace httpserver::requests
// http