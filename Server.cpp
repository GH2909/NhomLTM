#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Data/StockData.h"

#pragma comment(lib, "ws2_32.lib")

std::vector<SOCKET> clients;
std::mutex clientsMutex;
bool serverRunning = true;

// Gửi dữ liệu chứng khoán cho tất cả client
void broadcastStockData() {
    while (serverRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // gửi mỗi 1 giây
        std::string data = stockData();

        std::lock_guard<std::mutex> lock(clientsMutex);
        for (auto it = clients.begin(); it != clients.end(); ) {
            SOCKET client = *it;
            int sent = send(client, data.c_str(), (int)data.size(), 0);
            if (sent == SOCKET_ERROR) {
                closesocket(client);
                it = clients.erase(it); // xóa client không hợp lệ
            } else {
                ++it;
            }
        }
    }
}

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesRead;

    while (true) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) break;

        buffer[bytesRead] = '\0';
        std::string command(buffer);

        if (command.rfind("ADD ", 0) == 0) {
            std::string stock = command.substr(4);
            addCK(stock);
        }
        else if (command.rfind("REMOVE ", 0) == 0) {
            std::string stock = command.substr(7);
            removeCK(stock);
        }
        // GET lệnh client vẫn được server gửi định kỳ, nên không cần xử lý ở đây
    }

    closesocket(clientSocket);
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is running on port 8080...\n";

    // Thread gửi dữ liệu định kỳ
    std::thread broadcaster(broadcastStockData);
    broadcaster.detach();

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) continue;

        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.push_back(clientSocket);

        std::thread(handleClient, clientSocket).detach();
    }

    serverRunning = false;
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
