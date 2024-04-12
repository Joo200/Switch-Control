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

#include "ConfigurationServer.h"

#include <memory>

#include "config/ConfigurationStorage.h"
#include "requests/ChannelConfig.h"
#include "requests/ChannelStatus.h"
#include "requests/EmbedFileGetRequest.h"
#include "requests/WiFiConfig.h"
#include "webserver/requests/Status.h"


static bool uri_match(const char *uri1, const char *uri2, size_t len2) {
    const std::string kAll = "*";
    const std::string kAllNoApi = "!/api";
    const std::string kApiPath = "/api";

    if (kAll == uri1) {
        return true;
    }

    if (kAllNoApi == uri1) {
        std::string requested(uri2);
        return !requested.starts_with(kApiPath);
    }

    return strlen(uri1) == len2 &&          // First match lengths
           (strncmp(uri1, uri2, len2) == 0);   // Then match actual URIs
}

namespace httpserver {

ConfigurationServer::ConfigurationServer(config::ConfigurationStorage &storage, wifi::WiFiController &wifi, OperationController &ctrl)
    : storage_(storage), wifi_(wifi), ctrl_(ctrl) {}

ConfigurationServer::~ConfigurationServer() { stop(); }

bool ConfigurationServer::start() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8000;
    config.max_uri_handlers = 12;
    config.uri_match_fn = &uri_match;
    bool success = httpd_start(&server_, &config) == ESP_OK;

    handler_.push_back(std::make_unique<OptionsHandler>(*this));
    handler_.push_back(std::make_unique<requests::ConfigSet>(*this));
    handler_.push_back(std::make_unique<requests::ConfigGet>(*this));
    handler_.push_back(std::make_unique<requests::StatusGet>(*this));
    handler_.push_back(std::make_unique<requests::ChannelStatusGet>(*this));
    handler_.push_back(std::make_unique<requests::ChannelStatusPost>(*this));
    handler_.push_back(std::make_unique<requests::WiFiGet>(*this));
    handler_.push_back(std::make_unique<requests::WiFiSet>(*this));
    handler_.push_back(std::make_unique<requests::EmbedFileGetRequest>(*this, requests::EmbedFileConfiguration::kFavicon));
    handler_.push_back(std::make_unique<requests::EmbedFileGetRequest>(*this, requests::EmbedFileConfiguration::kIndexHtml));

    return success;
}

void ConfigurationServer::stop() {
    if (server_ != nullptr) {
        httpd_stop(server_);
        server_ = nullptr;
    }
}

}  // namespace httpserver
