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

#ifndef SWITCHCONTROL_WEBSERVER_ABSTRACTREQUESTHANDLER_H
#define SWITCHCONTROL_WEBSERVER_ABSTRACTREQUESTHANDLER_H

#include <esp_http_server.h>

#include <string>

#include "ConfigurationServer.h"

namespace httpserver {
class AbstractRequestHandler {
   public:
    AbstractRequestHandler(ConfigurationServer &srv, const char *path, http_method method);
    virtual ~AbstractRequestHandler() = default;

   protected:
    static std::string getParamKey(const std::string &val, httpd_req_t *req);

    static nlohmann::json getJsonBody(httpd_req_t *req);

    static void sendEmptySuccess(httpd_req_t *req);
    static void sendJsonAnswer(httpd_req_t *req, const nlohmann::json &j);
    static void sendJsonError(httpd_req_t *req, const std::string &err);

   public:
    ConfigurationServer &srv_;
    httpd_uri_t desc_{};

    virtual esp_err_t handleRequest(httpd_req_t *req) = 0;
};

class OptionsHandler : public AbstractRequestHandler {
public:
    explicit OptionsHandler(ConfigurationServer &srv) : AbstractRequestHandler(srv, "*", HTTP_OPTIONS) {}

    esp_err_t handleRequest(httpd_req_t *req) {
        httpd_resp_set_status(req, "200");
        httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }
};

}  // namespace httpserver

#endif  // SWITCHCONTROL_WEBSERVER_ABSTRACTREQUESTHANDLER_H
