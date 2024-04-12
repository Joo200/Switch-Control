idf_component_register(
        SRCS
        "main.cpp"

        "config/ButtonConfig.cpp"
        "config/ConfigurationStorage.cpp"
        "config/GpioConfig.cpp"
        "config/ServoConfig.cpp"
        "config/WiFiConfig.cpp"

        "controller/OperationController.cpp"

        "io/ServoOutChannel.cpp"
        "io/SmartButtonChannel.cpp"

        "webserver/ConfigurationServer.cpp"
        "webserver/AbstractRequestHandler.cpp"

        "webserver/requests/ChannelConfig.cpp"
        "webserver/requests/ChannelStatus.cpp"
        "webserver/requests/EmbedFileGetRequest.cpp"
        "webserver/requests/Status.cpp"
        "webserver/requests/WiFiConfig.cpp"

        "wifi/WiFiController.cpp"
        INCLUDE_DIRS .
        REQUIRES
        esp_http_server driver esp_wifi nvs_flash esp_http_client spiffs esp_app_format
        EMBED_TXTFILES
        ../web/dist/index.html
        EMBED_FILES
        ../web/dist/favicon.ico
)
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++20)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)
target_link_libraries(${COMPONENT_LIB} PRIVATE nlohmann_json::nlohmann_json)
