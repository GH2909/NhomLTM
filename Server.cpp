#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm> // Thêm để dùng std::remove
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Data/StockData.h"

#pragma comment(lib, "ws2_32.lib")

std::vector<SOCKET> clients;
std::mutex clientsMutex;

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesRead;

    while (true) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) break;

        buffer[bytesRead] = '\0';
        std::string command(buffer);

        if (command == "GET") {
            std::string data = stockData();
            send(clientSocket, data.c_str(), (int)data.size(), 0);
        }
        else if (command.rfind("ADD ", 0) == 0) {
            std::string stock = command.substr(4);
            addCK(stock);
        }
        else if (command.rfind("REMOVE ", 0) == 0) {
            std::string stock = command.substr(7);
            removeCK(stock);
        }
    }

    closesocket(clientSocket);

    // Xóa client khỏi danh sách
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    std::cout << "Server is running on port 8080...\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back(clientSocket);
        }
        std::thread(handleClient, clientSocket).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}