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

#include "SrcpSocket.h"

#include "esp_log.h"
#include "lwip/sockets.h"
#include "driver/gpio.h"

#include <cstring>
#include <thread>

namespace srcp {

static const char *TAG = "SRCP";
static const int SRCP_PORT = 4303;

SrcpSocket::SrcpSocket() = default;

void SrcpSocket::start() {
    int listenSock = socket(AF_INET6, SOCK_STREAM, IPPROTO_IPV6);
    if (listenSock < 0) {
        ESP_LOGE(TAG, "Failed to create socket");
        return;
    }

    sockaddr_in6 serverAddr = {};
    serverAddr.sin6_family = AF_INET6;
    serverAddr.sin6_port = htons(SRCP_PORT);
    bzero(&serverAddr.sin6_addr.un, sizeof(serverAddr.sin6_addr.un));

    if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        ESP_LOGE(TAG, "Bind failed");
        close(listenSock);
        return;
    }

    if (listen(listenSock, 1) < 0) {
        ESP_LOGE(TAG, "Listen failed");
        close(listenSock);
        return;
    }

    ESP_LOGI(TAG, "SRCP server listening on port %d", SRCP_PORT);

    while (true) {
        sockaddr_in6 clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int clientSock = accept(listenSock, (sockaddr *)&clientAddr, &addrLen);
        if (clientSock < 0) continue;

        std::thread(&SrcpSocket::handleClient, this, clientSock).detach();
    }
}

void SrcpSocket::handleClient(int clientSock) {
    send(clientSock, "201 SRCP ESP ready\n", 20, 0);

    char buffer[128];
    while (true) {
        int len = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) break;
        buffer[len] = '\0';
        processCommand(clientSock, std::string(buffer));
    }

    close(clientSock);
}

void SrcpSocket::processCommand(int sock, const std::string &cmd) {
    ESP_LOGI(TAG, "Received: %s", cmd.c_str());

    int addr, port, value;
    if (sscanf(cmd.c_str(), "SET GA %d %d %d", &addr, &port, &value) == 3) {
        ESP_LOGI(TAG, "Parsed GA addr=%d port=%d value=%d", addr, port, value);
        gpio_set_level(TURNOUT_GPIO, value);
        std::string response = "200 OK\n";
        send(sock, response.c_str(), response.length(), 0);
    } else {
        std::string response = "500 Syntax Error\n";
        send(sock, response.c_str(), response.length(), 0);
    }
}

} // srcp