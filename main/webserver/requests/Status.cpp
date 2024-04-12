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

#include "Status.h"

#include <esp_chip_info.h>
#include <esp_app_desc.h>

NLOHMANN_JSON_SERIALIZE_ENUM(esp_chip_model_t, {{CHIP_ESP32, "ESP32"},
#if 0
                                                {CHIP_ESP32S2, "ESP32-S2"},
                                                {CHIP_ESP32S3, "ESP32-S3"},
                                                {CHIP_ESP32C3, "ESP32-C3"},
                                                {CHIP_ESP32C2, "ESP32-C2"},
                                                {CHIP_ESP32C6, "ESP32-C6"},
                                                {CHIP_ESP32H2, "ESP32-H2"},
                                                {CHIP_ESP32P4, "ESP32-P4"},
                                                {CHIP_ESP32C61, "ESP32-C61"},
#endif
                                                {CHIP_POSIX_LINUX, "LINUX-SIM"}})

namespace httpserver::requests {
inline static const char *kStatusPath = "/api/status";

StatusGet::StatusGet(ConfigurationServer &srv) : AbstractRequestHandler(srv, kStatusPath, HTTP_GET) {}

static nlohmann::json getChipInfo() {
    nlohmann::json chipinfo;
    esp_chip_info_t chip;
    esp_chip_info(&chip);
    chipinfo["model"] = chip.model;
    chipinfo["cores"] = chip.cores;
    chipinfo["revision"] = chip.revision;
    chipinfo["features"] = chip.features;
    return chipinfo;
}

static nlohmann::json getAppInfo() {
    nlohmann::json app;
    const esp_app_desc_t *data = esp_app_get_description();
    app["name"] = data->project_name;
    app["version"] = data->version;
    app["idf-version"] = data->idf_ver;
    app["sha256"] = esp_app_get_elf_sha256_str();
    app["time"] = data->time;
    app["date"] = data->date;
    return app;
}

esp_err_t StatusGet::handleRequest(httpd_req_t *req) {
    nlohmann::json status;

    status["wifi"] = srv_.getWifi().getStatus();
    status["app"] = getAppInfo();
    status["chip"] = getChipInfo();

    sendJsonAnswer(req, status);
    return ESP_OK;
}

}  // namespace httpserver::requests
