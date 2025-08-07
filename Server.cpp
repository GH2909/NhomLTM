#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <atomic>
#include <windows.h> 
#include "Data/StockData.h"
#include <fcntl.h>
#include <io.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define PORT 9999

vector<SOCKET> clients;
mutex clientMutex;
atomic<bool> serverRunning(true);

// Gửi dữ liệu đến 1 client
void sendToClient(SOCKET client, const string& data) {
    send(client, data.c_str(), data.size(), 0);
}

// Gửi dữ liệu định kỳ đến tất cả client
void broadcastStockData() {
    while (serverRunning) {
        string data = stockData();

        lock_guard<mutex> lock(clientMutex);
        for (auto it = clients.begin(); it != clients.end(); ) {
            if (send(*it, data.c_str(), data.size(), 0) == SOCKET_ERROR) {
                cout << "[!] Client disconnected\n";
                closesocket(*it);
                it = clients.erase(it);
            } else {
                ++it;
            }
        }

        this_thread::sleep_for(chrono::seconds(5));
    }
}

// Xử lý client riêng biệt
void handleClient(SOCKET clientSocket) {
    cout << "[+] New client connected\n";

    char buffer[256];
    while (true) {
        int ret = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (ret <= 0) {
            cout << "[!] Client disconnected\n";
            break;
        }
    }

    lock_guard<mutex> lock(clientMutex);
    closesocket(clientSocket);
    clients.erase(remove(clients.begin(), clients.end(), clientSocket), clients.end());
}


int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U16TEXT);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "[-] Socket creation failed.\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "[-] Bind failed.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    listen(serverSocket, SOMAXCONN);
    cout << "[*] Server is listening on port " << PORT << "...\n";

    thread broadcaster(broadcastStockData);

    while (serverRunning) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket != INVALID_SOCKET) {
            {
                lock_guard<mutex> lock(clientMutex);
                clients.push_back(clientSocket);
            }
            thread(handleClient, clientSocket).detach();
        }
    }

    // Cleanup
    broadcaster.join();
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
