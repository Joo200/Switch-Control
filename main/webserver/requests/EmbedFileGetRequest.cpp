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

#include "EmbedFileGetRequest.h"

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");

namespace httpserver::requests {
const EmbedFileConfiguration EmbedFileConfiguration::kIndexHtml = {"!/api", "text/html; charset=utf-8",
                                                                   index_html_start, index_html_end};

const EmbedFileConfiguration EmbedFileConfiguration::kFavicon = {"/favicon.ico", "image/x-icon", favicon_ico_start,
                                                                 favicon_ico_end};

EmbedFileGetRequest::EmbedFileGetRequest(httpserver::ConfigurationServer &srv, const EmbedFileConfiguration &cfg)
    : AbstractRequestHandler(srv, cfg.path, HTTP_GET), cfg_(cfg) {}

esp_err_t EmbedFileGetRequest::handleRequest(httpd_req_t *req) {
    httpd_resp_set_type(req, cfg_.type);
    httpd_resp_send(req, (char *)cfg_.start_data, cfg_.end_data - cfg_.start_data);
    return ESP_OK;
}
}  // namespace httpserver::requests
