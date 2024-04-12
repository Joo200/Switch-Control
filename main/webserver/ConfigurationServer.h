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

#ifndef SWITCHCONTROL_WEBSERVER_CONFIGURATIONSERVER_H
#define SWITCHCONTROL_WEBSERVER_CONFIGURATIONSERVER_H

#include <esp_http_server.h>

#include <memory>

#include "config/ConfigurationStorage.h"
#include "config/GpioConfig.h"
#include "controller/OperationController.h"
#include "wifi/WiFiController.h"

namespace httpserver {
class AbstractRequestHandler;

class ConfigurationServer {
   public:
    explicit ConfigurationServer(config::ConfigurationStorage &storage, wifi::WiFiController &wifi, OperationController &ctrl);

    ~ConfigurationServer();

    bool start();

    void stop();

    [[nodiscard]] httpd_handle_t handle() { return server_; }

    [[nodiscard]] config::ConfigurationStorage &getStorage() { return storage_; }
    [[nodiscard]] wifi::WiFiController &getWifi() { return wifi_; }
    [[nodiscard]] OperationController &getController() { return ctrl_; }

   private:
    std::vector<std::unique_ptr<AbstractRequestHandler>> handler_;
    config::ConfigurationStorage &storage_;
    wifi::WiFiController &wifi_;
    OperationController &ctrl_;
    httpd_handle_t server_{nullptr};
};

}  // namespace httpserver

#endif  // SWITCHCONTROL_WEBSERVER_CONFIGURATIONSERVER_H
