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

#include <esp_log.h>

#include "AbstractRequestHandler.h"
#include "ConfigurationServer.h"

namespace httpserver {
#define CORS_HEADER

static esp_err_t internalHandle(httpd_req_t *req) {
    ESP_LOGD("http", "Entering internal handle callback");
#ifdef CORS_HEADER
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "*");
#endif
    auto val = reinterpret_cast<AbstractRequestHandler *>(req->user_ctx);
    auto ret = val->handleRequest(req);
    ESP_LOGD("http", "Exiting internal handle callback %d", ret);
    return ret;
}

AbstractRequestHandler::AbstractRequestHandler(ConfigurationServer &srv, const char *path, http_method method)
    : srv_(srv) {
    memset(&desc_, 0, sizeof(httpd_uri_t));
    desc_.uri = path;
    desc_.method = method;
    desc_.user_ctx = this;
    desc_.handler = &internalHandle;
    httpd_register_uri_handler(srv.handle(), &desc_);
}

std::string AbstractRequestHandler::getParamKey(const std::string &val, httpd_req_t *req) {
    size_t querySize = httpd_req_get_url_query_len(req);
    auto buffer = std::make_unique<char[]>(querySize + 1);
    if (httpd_req_get_url_query_str(req, buffer.get(), querySize + 1) != ESP_OK) {
        return "";
    }

    if (httpd_query_key_value(buffer.get(), val.c_str(), buffer.get(), querySize) == ESP_OK) {
        return {buffer.get()};
    }
    return "";
}

nlohmann::json AbstractRequestHandler::getJsonBody(httpd_req_t *req) {
    auto buf = std::make_unique<char[]>(req->content_len + 1);
    int ret = httpd_req_recv(req, buf.get(), req->content_len);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            ESP_LOGW("http", "request timeout");
            throw std::runtime_error("request timeout");
        }
        ESP_LOGW("http", "failed to receive buffer");
        throw std::runtime_error("failed to receive buffer");
    }
    return nlohmann::json::parse(buf.get(), buf.get() + req->content_len, nullptr, false);
}

void AbstractRequestHandler::sendEmptySuccess(httpd_req_t *req) {
    httpd_resp_set_status(req, "204");
    httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
}

void AbstractRequestHandler::sendJsonAnswer(httpd_req_t *req, const nlohmann::json &j) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, j.dump().c_str(), HTTPD_RESP_USE_STRLEN);
}

void AbstractRequestHandler::sendJsonError(httpd_req_t *req, const std::string &err) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "401");
    nlohmann::json body = {{"error", err}};
    httpd_resp_send(req, body.dump().c_str(), HTTPD_RESP_USE_STRLEN);
}
}  // namespace httpserver